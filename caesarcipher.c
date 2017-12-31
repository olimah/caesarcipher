/*
 * Copyright (c) 2016-2017 Oliver Mahmoudi
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
#include <unistd.h>

#define LOWER 32
#define UPPER 126
#define	LOOP (UPPER-LOWER+1)
#define OPTSTRING "dehi:o:ps:"
#define MAXINPUT 1024

long long int shift;
char *file_to_read = NULL , *file_to_write = NULL;
int pflag;

/* Functions */
void write_to_file(char *file_to_read, char *file_to_write, char *_operation, long long int _shift);
void print_ascii_table();
void print_commands();
void print_shift_table(long long int _shift);
void encode_message(char *_message, long long int _shift);
void decode_message(char *_message, long long int _shift);
void cc_shell();
void usage();

/*
 * As far as our cypher operations are concerned, there are four possibilitites here:
 *
 * 1. The shift value is positive and we are encrypting:
 * Take the modulus of the shift value and the LOOP. When encrypting, there is a chance,
 * that we count above the UPPER boundary. In this case, subtract the LOOP value from
 * the shifted char.
 *
 * 2. The shift value is positive and we are decrypting:
 * Take the modulus of the shift value and the LOOP. When decrypting (subtracting), there
 * is a chance, that we end up with a shift_value 1. below the LOWER boundary and below 
 * zero or that out shift_value is 2. below the LOWER boundary but still above or equal 
 * to zero. In case number 1. If after the initial decryption operation, shifted_char
 * is below zero, then multiply it with -1 and set shifted_char = LOOP - shifted_char.
 * In case number 2. If after the initial decryption operation, shifted_char is above or
 * equal to zero, then set shifted_char = LOOP + shifted_char.
 *
 * 3. The shift value is negative and we are encrypting:
 * In this case, take the modulus of the negative shift value and the LOOP. Then multiply
 * the (still) negative shift value with -1 and subtract it from the LOOP value. Afterwards
 * continue as you would, if a positive shift value has been passed. See 1.
 *
 * 4. The shift value is negative and we are decrypting:
 * In this case, take the modulus of the negative shift value and the LOOP. Then multiply
 * the (still) negative shift value with -1 and subtract it from the LOOP value. Afterwards
 * continue as you would, if a positive shift value has been passed. See 2.
 */

void
write_to_file(char *file_to_read, char *file_to_write, char *_operation, long long int _shift)
{
	int n, shift, shifted_char;
	FILE *fp_read, *fp_write;

	shift = _shift % LOOP;	

	if (shift < 0) {
		shift = shift*(-1);
		shift = LOOP-shift;
	}

	if ((fp_read = fopen(file_to_read, "r")) == NULL) {
		printf("error opening file: %s", file_to_read);
		exit(EXIT_FAILURE);
	}
	
	if (file_to_write != NULL) {
		if ((fp_write = fopen(file_to_write, "w")) == NULL) {
			printf("error opening file: %s", file_to_write);
			exit(EXIT_FAILURE);
		}
	}

	if (strcmp(_operation, "decrypt") == 0) {
		while ((n = fgetc(fp_read)) != EOF ) {
			if (n == '\n') {
				if (file_to_write != NULL)
					putc(n, fp_write);
				else
					putc(n, stdout);
			}
			else if (n == ' ' && pflag == 1) {
				if (file_to_write != NULL)
					putc(n, fp_write);
				else
					putc(n, stdout);
			}
			else {
				shifted_char = n - shift;

			if(shifted_char < LOWER) {
				if(shifted_char < 0) {
					shifted_char = shifted_char * (-1);
					shifted_char = LOOP - shifted_char;
				}
				else
					shifted_char = LOOP + shifted_char;
			}

			if (file_to_write != NULL)
				putc(shifted_char, fp_write);
			else
				putc(shifted_char, stdout);
			}
		}
	}
	else if (strcmp(_operation, "encrypt") == 0) {
		while ((n = fgetc(fp_read)) != EOF ) {
			if (n == '\n') {
				if (file_to_write != NULL)
					putc(n, fp_write);
				else
					putc(n, stdout);
			}
			else if (n == ' ' && pflag == 1) {
				if (file_to_write != NULL)
					putc(n, fp_write);
				else
					putc(n, stdout);
			}
			else {
				shifted_char = n + shift;

				if(shifted_char > UPPER)
					shifted_char = shifted_char - LOOP;

				if (file_to_write != NULL)
					putc(shifted_char, fp_write);
				else
					putc(shifted_char, stdout);
			}
		}
	}

	fclose(fp_read);

	if (file_to_write != NULL)
		fclose(fp_write);
}

void
print_ascii_table()
{
	int i;

	printf("Printing standard ascii table\n");
	printf("Decimcal:\t\t\tHex:\t\t\tAscii:\n");
	for(i=32; i <= 126; i++) {
		printf("%9d\t\t\t%2x\t\t\t%5c\n",i,i,i);
	}
}

void
print_commands()
{
	printf("casearcipher commands:\n");
	printf("ascii - print ascii table\n");
	printf("decrypt - decrypt a message\n");
	printf("encrypt - encrypt a message\n");
	printf("message - specify a message to decrypt/encrypt\n");
	printf("print - print shift table\n");
	printf("quit - quit caesarciper\n");
	printf("shift - set a new shift value\n");
	printf("spaces - turn preserve spaces on/off\n");
	printf("help - print this help menu\n");
}

void
print_shift_table(long long int _shift)
{
	int i, shift, shift_val, shift_val_new;

	printf("Original:\t\tShifted by %lli:\n", _shift);

	shift_val_new = LOWER;
	shift = _shift % LOOP;
	shift_val = shift;

	for(i=LOWER; i <= UPPER; i++) {
		shift_val = i + shift_val;

		if(shift_val > UPPER) {
			printf("%c\t\t\t%c\n", i, shift_val_new);
			shift_val_new++;
		}
		else {
			printf("%c\t\t\t%c\n", i, shift_val);
			/* reset shift_val to its original value for the next execution */
			shift_val = shift;
		}
	}
}

void
encode_message(char *_message, long long int _shift)
{
	char *message = _message;
	int i, shift, shifted_char;

	shift = _shift % LOOP;

	/*
	 * Maybe we were passed a negative _shift value.
	 */

	if (shift < 0) {
		shift = shift*(-1);
		shift = LOOP-shift;
	}

	putchar('\n');
	printf("Encoding message with a shiftvalue of: %lli\n", _shift);
	printf("The original message is:\n%s\n", message);
	printf("The encoded message is:\n");
	for(i = 0; i < strlen(message); i++) {
		shifted_char = message[i] + shift;

		if(shifted_char > UPPER)
			shifted_char = shifted_char - LOOP;

		if (message[i] == ' ' && pflag == 1)
			putchar(' ');
		else
			printf("%c", shifted_char);
	}
	putchar('\n');
}

void
decode_message(char *_message, long long int _shift)
{
	char *message = _message;
	int i, shift, shifted_char;

	shift = _shift % LOOP;

	/*
	 * Maybe we were passed a negative _shift value.
	 */ 

	if (shift < 0) {
		shift = shift*(-1);
		shift = LOOP-shift;
	}

	putchar('\n');
	printf("Decoding message with a shiftvalue of: %lli\n", _shift);
	printf("The encoded message is:\n%s\n", message);
	printf("The decoded message is:\n");
	for(i = 0; i < strlen(message); i++) {
		shifted_char = message[i] - shift;

		if(shifted_char < LOWER) {
			if(shifted_char < 0) {
				shifted_char = shifted_char * (-1);
				shifted_char = LOOP - shifted_char;
			}
			else
				shifted_char = LOOP + shifted_char;
		}

		if (message[i] == ' ' && pflag == 1)
			putchar(' ');
		else
			printf("%c", shifted_char);
	}
	putchar('\n');
}

void
cc_shell()
{
	char shiftval[MAXINPUT];	/* convert userinput via fgets */
	char userinput[MAXINPUT];	/* command to be run */
	char usermessage[MAXINPUT]; /* this gets passed to *message */
	char *message = NULL;		/* message to be decoded/encoded */
	char *str, *endptr;			/* for strtoll */
	int i;

	printf("Welcome to caesarcipher!\n");
	printf("Using a shift value of: %d\n", shift);
	
	if(pflag == 0)
		printf("Spaces will be decoded/encoded.\n");
	else
		printf("Spaces will be preserved.\n");

	printf("Enter 'help' to see a list of commands. Enter 'exit' to quit.\n");
	for (;;) {
		printf("cc> ");
		fgets(userinput, MAXINPUT, stdin);

		/* 
		 * Reset counter and eliminate the newline so we can process the argument
		 */		

		i = 0; 
		while(userinput[i] != '\n')
			i++;

		userinput[i] = '\0';

		/*
 		 * Process our argument and call the corresponding function
 		 */

		if ((strcmp(userinput, "ascii")) == 0) {
			print_ascii_table();
		}
		else if ((strcmp(userinput, "help")) == 0) {
			print_commands();
		}
		else if ((strcmp(userinput, "decrypt")) == 0) {
			if (message == NULL) {
				printf("Please specify a message to decrypt: \n");
				fgets(usermessage, MAXINPUT, stdin);
				i = 0; 
				while(usermessage[i] != '\n')
					i++;

				usermessage[i] = '\0';
				message = usermessage;
			}
			else {
				printf("You are about to decrypt the following message with a shiftvalue of: %lli\n", shift);
				printf("%s\n", usermessage);
				do {
					printf("Proceed (y/n)? ");
					fgets(userinput, MAXINPUT, stdin);
					i = 0; 
					while(userinput[i] != '\n')
						i++;

					userinput[i] = '\0';
				} while (((strcmp(userinput, "y")) != 0) && ((strcmp(userinput, "n")) != 0) 
						&& ((strcmp(userinput, "yes")) != 0) && ((strcmp(userinput, "no")) != 0)
						&& ((strcmp(userinput, "Yes")) != 0) && ((strcmp(userinput, "No")) != 0));

				if ((strcmp(userinput, "y")) == 0)
					message = usermessage;
				else {
					/*
					 * The user chose "no". Let him enter a new message.
					 */

					printf("Please specify a message to decrypt: \n");
					fgets(usermessage, MAXINPUT, stdin);
					i = 0; 
					while(usermessage[i] != '\n')
						i++;

					usermessage[i] = '\0';
					message = usermessage;

					/*
					 * We also require the user to set a new shift value.
					 */

					printf("Set the new shift value: ");
					fgets(shiftval, MAXINPUT, stdin);
					str = shiftval;
					shift = strtoll(str, &endptr, 10);
				}

			}
			decode_message(message, shift);
		}
		else if ((strcmp(userinput, "encrypt")) == 0) {
			if (message == NULL) {
				printf("Please specify a message to encrypt: \n");
				fgets(usermessage, MAXINPUT, stdin);
				i = 0; 
				while(usermessage[i] != '\n')
					i++;

				usermessage[i] = '\0';
				message = usermessage;
			}
			else {
				printf("You are about to encrypt the following message with a shiftvalue of: %lli\n", shift);
				printf("%s\n", usermessage);

				do {
					printf("Proceed (y/n)? ");
					fgets(userinput, MAXINPUT, stdin);
					i = 0; 

					while(userinput[i] != '\n')
						i++;

					userinput[i] = '\0';
				} while (((strcmp(userinput, "y")) != 0) && ((strcmp(userinput, "n")) != 0) 
						&& ((strcmp(userinput, "yes")) != 0) && ((strcmp(userinput, "no")) != 0)
						&& ((strcmp(userinput, "Yes")) != 0) && ((strcmp(userinput, "No")) != 0));

				if ((strcmp(userinput, "y")) == 0)
					message = usermessage;
				else {

					/*
					 * The user chose "no". Let him enter a new message.
					 */

					printf("Please specify a message to encrypt: \n");
					fgets(usermessage, MAXINPUT, stdin);
					i = 0; 
					while(usermessage[i] != '\n')
						i++;

					usermessage[i] = '\0';
					message = usermessage;

					/*
					 * We also require the user to set a new shift value.
					 */

					printf("Set the new shift value: ");
					fgets(shiftval, MAXINPUT, stdin);
					str = shiftval;
					shift = strtoll(str, &endptr, 10);
				}

			}
			encode_message(message, shift);
		}
		else if ((strcmp(userinput, "message")) == 0) {
			printf("Please specify a message: ");
			fgets(usermessage, MAXINPUT, stdin);

			i = 0; 
			while(usermessage[i] != '\n')
				i++;

			usermessage[i] = '\0';
			message = usermessage;
		}
		else if ((strcmp(userinput, "print")) == 0) {
			print_shift_table(shift);
		}
		else if ((strcmp(userinput, "exit")) == 0)
			break;
		else if ((strcmp(userinput, "quit")) == 0)
			break;
		else if ((strcmp(userinput, "shift")) == 0) {
			printf("Set the new shift value: ");
			fgets(shiftval, MAXINPUT, stdin);
			str = shiftval;
			shift = strtoll(str, &endptr, 10);
		}
		else if ((strcmp(userinput, "spaces")) == 0) {
			if (pflag == 0) {
				pflag = 1;
				printf("Spaces will be preserved when decoding/encoding.\n");
			}
			else {
				pflag = 0;
				printf("Spaces will be decoded/encoded.\n");
			}
		}
		else
			printf("unkown command\n");
	} /* for (;;) */

	printf("Quitting ceasarcipher\n");
	exit(EXIT_SUCCESS);
}

void
usage()
{
	printf("usage:\n");
	printf("caesarcipher [-d|-e] [-p] [-s shiftvalue] [-i input_file] [-o output_file] [message ...]\n");
}

int
main(int argc, char *argv[])
{
	int opt, dflag, eflag, iflag, oflag;

	pflag = shift = dflag = eflag = iflag = oflag = 0;

	while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
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
			file_to_read = optarg;
			iflag = 1;
			break;
		case 'o':
			file_to_write = optarg;
			oflag = 1;
			break;
		case 's':
			shift = atoi(optarg);
			break;
		case 'p':
			pflag = 1;
			break;
		default:
			usage();
			return(EXIT_FAILURE);
		}
	}

	if (((dflag == 0) && (eflag == 0) && (optind < argc))) {
		printf("Please choose to either decrypt or encrypt your message!\n");
		usage();
		return(EXIT_FAILURE);
	}
	else if ((dflag == 1) && (eflag == 1)) {
		printf("Cannot decrypt and encrypt at the same time!\n");
		usage();
		return(EXIT_FAILURE);
	}
	else if ((dflag == 0) && (eflag == 0) && (optind == argc))
		cc_shell();
	else if ((dflag == 1) && (optind < argc)) {
		printf("Welcome to caesarcipher!\n");
		while (optind < argc)
			decode_message(argv[optind++], shift);

		return(EXIT_SUCCESS);
	} 
	else if ((eflag == 1) && (optind < argc)) {
		printf("Welcome to caesarcipher!\n");
		while (optind < argc)
			encode_message(argv[optind++], shift);

		return(EXIT_SUCCESS);
	}
	else if ((dflag == 1) && (iflag == 1) && (optind == argc)) {
		if (oflag == 1)
			write_to_file(file_to_read, file_to_write, "decrypt", shift);
		else
			write_to_file(file_to_read, NULL, "decrypt", shift);

		return(EXIT_SUCCESS);
	}
	else if ((eflag == 1) && (iflag == 1) && (optind == argc)) {
		if (oflag == 1)
			write_to_file(file_to_read, file_to_write, "encrypt", shift);
		else
			write_to_file(file_to_read, NULL, "encrypt", shift);

		return(EXIT_SUCCESS);
	}
	/* NOT REACHED */
	else {
		usage();
		return(EXIT_FAILURE);
	}
}
