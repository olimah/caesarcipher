/*
 * Copyright (c) 2016-2020 Oliver Mahmoudi
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions 
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define LOWER 0x20
#define UPPER 0x7E
#define	LOOP (UPPER-LOWER+1)
#define OPTSTRING "dehi:o:ps:"
#define MAXINPUT 1024
#define ENCRYPT "encrypt"
#define DECRYPT "decrypt"

int pflag, shiftval;
FILE *fp_read = NULL, *fp_write = NULL;

void read_write_file(char *);
int encrypt_char(int, int);
int decrypt_char(int, int);
void print_shift_table();
void print_ascii_table();
void print_commands();
void get_input(char *);
void cc_shell();
void usage();

/*
 * As far as our cypher operations are concerned, there are four possibilities here:
 *
 * 1. The shift value is positive and we are encrypting:
 * First take the modulus of the shift value and the LOOP to get the "natural" shift.
 * When encrypting (adding), there is a chance, that we count above the UPPER boundary.
 * In this case, subtract LOOP's value from the shifted char.
 *
 * 2. The shift value is positive and we are decrypting:
 * First take the modulus of the shift value and the LOOP to get the "natural" shift.
 * When decrypting (subtracting), there is a chance, that we end up with a shifted_char
 * below the LOWER boundary. When this is the case, add LOOP to shifted char, i.e. set:
 * shifted_char = LOOP + shifted_char.
 *
 * 3. The shift value is negative and we are encrypting:
 * Obtain the "natural" shift value by taking the modulus of the negative shift value
 * and the LOOP. Now add the negative shift value to LOOP's value and assign the result
 * to shift. From here on, continue as you would, if a positive shift value has been
 * passed. See 1.
 *
 * 4. The shift value is negative and we are decrypting:
 * Take the modulus of the negative shift value and the LOOP to get the "natural" shift.
 * Add the negative shift value to LOOP value and write the result back to shift. Now
 * continue as you would, if a positive shift value had been passed. See 2.
 */

void
read_write_file(char *_operation)
{
	int ch;

	if (!strcmp(_operation, DECRYPT)) {
		while ((ch = fgetc(fp_read)) != EOF )
			if (fp_write)
				putc(decrypt_char(ch, shiftval), fp_write);
			else
				putc(decrypt_char(ch, shiftval), stdout);
	} else if (!strcmp(_operation, ENCRYPT)) {
		while ((ch = fgetc(fp_read)) != EOF )
			if (fp_write)
				putc(encrypt_char(ch, shiftval), fp_write);
			else
				putc(encrypt_char(ch, shiftval), stdout);
	}

	fclose(fp_read);

	if (fp_write)
		fclose(fp_write);
}

int
encrypt_char(int _chartoenc, int _shiftval)
{
	int chartoenc, shifted_char;

	chartoenc = _chartoenc;

	if (chartoenc == '\n' || chartoenc == '\t' || (chartoenc == ' ' && pflag))
		return chartoenc;

	_shiftval = _shiftval % LOOP;

	/* Maybe we were passed a negative shiftvalue. */
	if (_shiftval < 0)
		_shiftval = LOOP + _shiftval;

	shifted_char = chartoenc + _shiftval;

	if(shifted_char > UPPER)
		shifted_char = shifted_char - LOOP;

	return shifted_char;
}

int
decrypt_char(int _chartodec, int _shiftval)
{
	int chartodec, shifted_char;

	chartodec = _chartodec;

	if (chartodec == '\n' || chartodec == '\t' || (chartodec == ' ' && pflag))
		return chartodec;

	_shiftval = _shiftval % LOOP;

	/* Maybe we were passed a negative shiftvalue. */
	if (_shiftval < 0)
		_shiftval = LOOP + _shiftval;

	shifted_char = chartodec - _shiftval;

	if(shifted_char < LOWER)
		shifted_char = LOOP + shifted_char;

	return shifted_char;
}

void
print_ascii_table()
{
	int i;
	printf("Printing standard ASCII table\n");
	printf("Decimcal:\t\t\tHex:\t\t\tASCII:\n");
	for(i = LOWER; i <= UPPER; i++)
		printf("%9d\t\t\t%2x\t\t\t%5c\n", i, i, i);
}

void
print_commands()
{
	printf("caesarcipher commands:\n");
	printf("ascii - print ASCII table\n");
	printf("decrypt - decrypt a message\n");
	printf("encrypt - encrypt a message\n");
	printf("shifttable - print shift table\n");
	printf("shiftvalue - set a new shiftvalue\n");
	printf("spaces - turn preserve spaces on/off\n");
	printf("help - print this help menu\n");
	printf("quit - quit caesarciper\n");
}

void
print_shift_table()
{
	int i;
	printf("Printing shifttable\n");
	printf("Original:\t\tShifted by %d:\n", shiftval);
	for(i = LOWER; i <= UPPER; i++)
		printf("%c\t\t\t%c\n", i, encrypt_char(i, shiftval));
}

void
get_input(char *_input) {
	fgets(_input, MAXINPUT, stdin);

	/* Eliminate the newline so we can process the input. */
	for(; *_input != '\n'; *_input++)
		;
	*_input = '\0';
}

void
cc_shell()
{
	int i;
	char userinput[MAXINPUT];

	printf("Welcome to caesarcipher!\n");
	printf("Using a shift value of: %d\n", shiftval);
	
	if(!pflag)
		printf("Spaces will be encrypted/decrypted.\n");
	else
		printf("Spaces will not be encrypted/decrypted.\n");

	printf("Enter 'help' to see a list of commands. Enter 'exit' to quit.\n");
	for (;;) {
		printf("cc> ");

		/*
 		 * Get userinput and process the command.
 		 */
		get_input(userinput);
		if (!strcmp(userinput, "ascii")) {
			print_ascii_table();
		} else if (!strcmp(userinput, DECRYPT)) {
			printf("Specify a message to decrypt: \n");
			get_input(userinput);
			printf("Decrypting message with a shiftvalue of: %d\n", shiftval);
			for(i = 0; i < strlen(userinput); i++)
				putchar(decrypt_char(userinput[i], shiftval));
			putchar('\n');
		} else if (!strcmp(userinput, ENCRYPT)) {
			printf("Specify a message to encrypt: \n");
			get_input(userinput);
			printf("Encrypting message with a shiftvalue of: %d\n", shiftval);
			for(i = 0; i < strlen(userinput); i++)
				putchar(encrypt_char(userinput[i], shiftval));
			putchar('\n');
		} else if (!strcmp(userinput, "shifttable"))
			print_shift_table();
		else if (!strcmp(userinput, "spaces")) {
			if (!pflag) {
				pflag = 1;
				printf("Spaces will not be encrypted/decrypted.\n");
			} else {
				pflag = 0;
				printf("Spaces will be encrypted/decrypted.\n");
			}
		} else if (!strcmp(userinput, "shiftvalue")) {
			printf("New shiftvalue: ");
			get_input(userinput);
			shiftval = atoi(userinput);
		} else if (!strcmp(userinput, ""))
			continue;
		else if (!strcmp(userinput, "exit") || !strcmp(userinput, "quit"))
			break;
		else if (!strcmp(userinput, "help"))
			print_commands();
		else
			printf("unkown command\n");
	} /* for (;;) */

	printf("Quitting caesarcipher\n");
	exit(EXIT_SUCCESS);
}

void
usage()
{
	printf("usage:\n");
	printf("caesarcipher [-d|-e] [-p] [-s shiftvalue] [-i inputfile] [-o outputfile] [message ...]\n");
}

int
main(int argc, char *argv[])
{
	int i, opt, dflag, eflag;

	pflag = shiftval = dflag = eflag = 0;

    static struct option long_options[] = {
  		{"decrypt",   no_argument, 0,  'd'},
	    {"encrypt",  no_argument, 0,  'e'},
    	{"help",  no_argument, 0,  'h'},
  		{"inputfile",   required_argument, 0,  'i'},
	    {"outputfile",  required_argument, 0,  'o'},
    	{"shiftvalue",  required_argument, 0,  's'},
    	{"preserve-spaces",  no_argument, 0,  'p'},
	    {0, 0, 0, 0}
    };

    while((opt = getopt_long(argc, argv, OPTSTRING, long_options, NULL)) != -1) {
		switch(opt) {
			case 'd':
				dflag = 1;
				break;
			case 'e':
				eflag = 1;
				break;
			case 'h':
				usage();			
				return(EXIT_SUCCESS);
				break;
			case 'i':
				if (!(fp_read = fopen(optarg, "r"))) {
					printf("error opening file: %s", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			case 'o':
				if (!(fp_write = fopen(optarg, "w"))) {
					printf("error opening file: %s", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			case 's':
				shiftval = atoi(optarg);
				break;
			case 'p':
				pflag = 1;
				break;
			default:
				usage();
				return(EXIT_FAILURE);
		}
	}

	if ((!dflag && !eflag && (optind < argc))) {
		printf("Please choose to either encrypt or decrypt your message!\n");
		usage();

		return(EXIT_FAILURE);
	} else if (dflag && eflag) {
		printf("Cannot encrypt and decrypt at the same time!\n");
		usage();

		return(EXIT_FAILURE);
	} else if (!dflag && !eflag && (optind == argc))
		cc_shell();
	else if (dflag && (optind < argc)) {
		while (optind < argc) {
			for(i = 0; argv[optind][i] != '\0'; i++)
				putchar(decrypt_char(argv[optind][i], shiftval));
			putchar('\n');
			optind++;
		}

		return(EXIT_SUCCESS);
	} else if (eflag && (optind < argc)) {
		while (optind < argc) {
			for(i = 0; argv[optind][i] != '\0'; i++)
				putchar(encrypt_char(argv[optind][i], shiftval));
			putchar('\n');
			optind++;
		}

		return(EXIT_SUCCESS);
	} else if (dflag && fp_read && (optind == argc)) {
		read_write_file(DECRYPT);

		return(EXIT_SUCCESS);
	} else if (eflag && fp_read && (optind == argc)) {
		read_write_file(ENCRYPT);

		return(EXIT_SUCCESS);
	} else {
		/* NOT REACHED */
		usage();
		return(EXIT_FAILURE);
	}
}
