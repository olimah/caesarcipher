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

#define	LOOP	(126-32+1)
#define OPTSTRING "dehi:o:s:"
#define MAXINPUT 1024

/* Functions */

/*
void write_to_file()
void print_ascii_table()
void print_commands()
void print_shift_table()
void encode_message()
void decode_message()
void usage()
*/

void
write_to_file(char *file_to_read, char *file_to_write, char *_operation, long long int _shift)
{
	int i, n, shift, shifted_char;
	FILE *fp_read, *fp_write;

	shift = _shift % LOOP;	

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
	else
		printf("Welcome to Caesar Cipher!\n");

	if (strcmp(_operation, "decrypt") == 0) {
		while ((n = fgetc(fp_read)) != EOF ) {
			if (n == '\n') {
				if (file_to_write != NULL)
					putc(n, fp_write);
				else
					putc(n, stdout);
			}
			else {
				shifted_char = n - shift;

			if(shifted_char < 32)
				shifted_char = LOOP - shifted_char;

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
			else {
				shifted_char = n + shift;

				if(shifted_char > 126)
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
	printf("Decimcal:\t\tHex:\t\t\tAscii:\n");
	for(i=32; i <= 126; i++) {
		printf("%9d\t\t\t%2x\t\t\t%5c\n",i,i,i);
	}
}

void
print_commands()
{
	printf("Casearcipher commands:\n");
	printf("ascii - print ascii table\n");
	printf("decrypt - decrypt a message\n");
	printf("encrypt - encrypt a message\n");
	printf("message - specify a message to decrypt/encrypt\n");
	printf("print - print shift table\n");
	printf("quit - quit caesarciper\n");
	printf("shift - set a new shift value\n");
	printf("help - print this help menu\n");
}

void
print_shift_table(long long int _shift)
{
	int i, shift, shift_val, shift_val_new;

	printf("Original:\t\tShifted by %lli:\n", _shift);

	shift_val_new = 32;
	shift = _shift % LOOP;
	shift_val = shift;

	for(i=32; i <= 126; i++) {
		shift_val = i + shift_val;

		if(shift_val > 126) {
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
/*
	if (shift < 0)
		shift = LOOP + shift;
*/	
	putchar('\n');
	printf("Encoding message with a shiftvalue of: %lli\n", _shift);
	printf("The original message is:\n%s\n", message);
	printf("The encoded message is:\n");
	for(i = 0; i < strlen(message); i++) {
		shifted_char = message[i] + shift;

		if(shifted_char > 126)
			shifted_char = shifted_char - LOOP;

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
	 * Maybe we were passed a positive _shift value.
	 */ 
/*
	if (shift > 0)
		shift = LOOP - shift;
*/
	putchar('\n');
	printf("Decoding message with a shiftvalue of: %lli\n", _shift);
	printf("The encoded message is:\n%s\n", message);
	printf("The decoded message is:\n");
	for(i = 0; i < strlen(message); i++) {
		shifted_char = message[i] - shift;

		if(shifted_char < 32) 
			shifted_char = LOOP - shifted_char;

		printf("%c", shifted_char);
	}
	putchar('\n');
}

void
usage()
{
	printf("usage:\n");
	printf("caesarcipher [-s shiftvalue] 						(1st Form)\n");
	printf("caesarcipher [-d || -e [-s shiftvalue] message] 			(2nd Form)\n");
	printf("caesarcipher [-d || -e [-s shiftvalue] -i input_file [-o output_file]] 	(3rd Form)\n");
}

int
main(int argc, char *argv[])
{
	int i, opt, dflag, eflag, iflag, oflag;
	long long int shift;
	char *message = NULL, *file_to_read = NULL , *file_to_write = NULL;
	char shiftval[MAXINPUT];	/* we get our shiftvalue by converting userinput obtained from fgets */
	char userinput[MAXINPUT], usermessage[MAXINPUT];
	char *str, *endptr;			/* for strtoll */

	i = shift = dflag = eflag = iflag = oflag = 0;

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
		default:
			usage();
			return(EXIT_FAILURE);
		}
	}

	/*
	 * Sanity checks
	 */ 

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
	/*
	 * 3rd Form: The user invoked caesarcipher with an input- and possibly an output file. If no output file
	 * is specified, then the result is written to stdout.
	 */
	else if ((dflag == 1) && (iflag == 1) && (optind == argc)) {
		if (oflag == 1)
			write_to_file(file_to_read, file_to_write, "decrypt", shift);
		else
			write_to_file(file_to_read, NULL, "decrypt", shift);

		return (EXIT_SUCCESS);
	}
	else if ((eflag == 1) && (iflag == 1) && (optind == argc)) {
		if (oflag == 1)
			write_to_file(file_to_read, file_to_write, "encrypt", shift);
		else
			write_to_file(file_to_read, NULL, "encrypt", shift);

		return (EXIT_SUCCESS);
	}
	/*
	 * 2nd Form: The user invoked caesarcipher with a message and decides to decrypt/encrypt it. 
	 * Call the appropriate function and exit.
	 */ 
	else if ((dflag == 1) && (optind < argc)) {
		printf("Welcome to Caesar Cipher!\n");
		while (optind < argc)
			decode_message(argv[optind++], shift);

		return (EXIT_SUCCESS);
	} 
	else if ((eflag == 1) && (optind < argc)) {
		printf("Welcome to Caesar Cipher!\n");
		while (optind < argc)
			encode_message(argv[optind++], shift);

		return(EXIT_SUCCESS);
	}
	/*
	 * 1st Form: The user invoked caesarcipher only with a shiftvalue.
	 * He is dropped into a shell like environment, where he can enter his commands.
	 */
	else if ((dflag == 0) && (eflag == 0) && (optind == argc)) {
		printf("Welcome to Caesar Cipher!\n");
		printf("Using a shift value of: %d\n", shift);
		printf("Enter 'help' to see a list of commands. Enter 'exit' to quit.\n");
		for (;;) {
			printf("cc> ");
			fgets(userinput, MAXINPUT, stdin);

			/* 
			 * reset counter and eliminate the newline so we can process the argument
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
			else
				printf("unkown command\n");
		} /* for (;;) */

		printf("Quitting ceasar cipher\n");
		return(EXIT_SUCCESS);
	}
	else {
		usage();
		return(EXIT_FAILURE);
	}
}
