#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* The RGB values of a pixel. */
struct Pixel // this is a structure that wil hold all info of the RGB values of a particular pixel
{
	int red;
	int green;
	int blue;
};

/* An image loaded from a PPM file. */
struct PPM // this is a structure that can hold all info of the PPM file
{
	int width;			  // this represents the integer number of columns
	int height;			  // this represents the integer number of rows
	int max;			  // this represents the maximum value the rgb colours that the pixels can hold
	struct Pixel *rgbArr; // this is a pointer to an array of type 'struct Pixel' that can contain pixel data
	char *format;		  // this will hold information of the 'magic number'/format of the ppm file
};

/* Reads an image from an open PPM file.
 * Returns a new struct PPM, or NULL if the image cannot be read. */
struct PPM *getPPM(FILE *f)
{
	struct PPM *thePpmFile;								   // this is the initiallization of a pointer to a struct PPM which will hold the contents of the opened PPM file f
	thePpmFile = (struct PPM *)malloc(sizeof(struct PPM)); // allocating memory to the pointer

	char ppmFormat[3];			// this is a string that will first read the ppm format of the file and store in in a string called ppmFormat
	fscanf(f, "%s", ppmFormat); // reads the first line of the open file to retrieve the ppm format

	int pwidth, pheight;
	fscanf(f, "%d %d", &pwidth, &pheight); // this stores the width and height of the ppm file which is the next line after the magic number in a file w/o comments

	int pmax;
	fscanf(f, "%d", &pmax); // reading MAX_VAL

	thePpmFile->width = pwidth;
	thePpmFile->height = pheight;
	thePpmFile->max = pmax;					// storing MAX, width and height for thePpmFile
	thePpmFile->format = strdup(ppmFormat); // use strdup to duplicate the read string on to thePpmFile

	if ((strcmp(thePpmFile->format, "P3")) != 0) // checking if format of PPM is "P3"
	{
		free(thePpmFile); // free the memory allocated for PPM struct and return null as the format isnt P3
		return NULL;
	}

	int row_count = pheight * pwidth; // no. of rows pixels in thePpmFile is the height * width of the read ppm file

	thePpmFile->rgbArr = (struct Pixel *)malloc(row_count * sizeof(struct Pixel));
	// allocating appropriate size using the malloc() function so that the rgbArr can hold all pixel values

	if (thePpmFile->rgbArr == NULL) // if the memory allocation wasnt a success we return NULL
	{
		free(thePpmFile);
		printf("ISSUE with MEMORY ALLOCATION!!");
		return NULL;
	}

	int pred, pgreen, pblue;

	for (int i = 0; i < row_count; i++) // uisng a for loop to iterate through the row_count pixels in the ppm file
	{
		fscanf(f, "%d %d %d", &pred, &pgreen, &pblue);
		thePpmFile->rgbArr[i].red = pred;
		thePpmFile->rgbArr[i].green = pgreen;
		thePpmFile->rgbArr[i].blue = pblue;
	}

	return thePpmFile; // returns the new struct PPM which is having the details of the required PPM file f
}

/* Write img to stdout in PPM format. */
void showPPM(const struct PPM *img)
{
	printf("%s\n", img->format);				// this will print the "magic number"/format
	printf("%d %d\n", img->width, img->height); // the width and height of the ppm file
	printf("%d\n", img->max);					// max_val
	int row_count = img->height * img->width;
	for (int i = 0; i < (row_count); i++) // for loop to print all the rgb values
	{
		printf("%d %d %d \n", img->rgbArr[i].red, img->rgbArr[i].green, img->rgbArr[i].blue);
	}
}

/* Opens and reads a PPM file, returning a pointer to a new struct PPM.
 * On error, prints an error message and returns NULL. */
struct PPM *readPPM(const char *filename)
{
	/* Open the file for reading */
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		fprintf(stderr, "File %s could not be opened.\n", filename);
		return NULL;
	}

	/* Load the image using getPPM */
	struct PPM *img = getPPM(f);

	/* Close the file */
	fclose(f);

	if (img == NULL)
	{
		fprintf(stderr, "File %s could not be read.\n", filename);
		return NULL;
	}

	return img;
}

int *bubbleSort(int arr[], int length) // using bubblesort algorithm to sort the array of random indices, this helps with the encode implementation
{
	int swapped; // this will tell us if we performed a swap or not
	int temp;	 // a temp value to perform the swap
	for (int i = 0; i < length - 1; i++)
	{
		swapped = 0; // setting it to false
		for (int j = 0; j < length - i - 1; j++)
		{
			if (arr[j] > arr[j + 1]) // when the current element is greater than the next element we peroform a swap
			{
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
				swapped = 1;
			}
		}
		if (swapped == 0)
		{
			break;
		}
	}
	return arr;
}

int *randArr(int length, int min, int max) // this is a function that creates an array of random integers of a specified from a range of min to max.
{
	int *theArr = (int *)malloc(length * sizeof(int));
	if (length < 0) // if the length is not a positive number greater than 0 we do not peroform encode.
	{
		printf("Error the lenght of the message given isnt greater than 0!!");
		free(theArr);
		return NULL;
	}

	for (int i = 0; i < length; i++)
	{
		int unique = 0; // a status so that we can check if the generated number is unique so that we prevent collisions with index
		while (!unique)
		{
			theArr[i] = min + rand() % (max - min + 1); // this generates a random number using rand() and is using the ranges specified
			unique = 1;									// setting unique status to true
			for (int j = 0; j < i; j++)					// a for loop to iterate through the generated array to check if the numbers generated are in fact unique
			// linear search
			{
				if (theArr[i] == theArr[j]) // checking for collision
				{
					unique = 0; // unique is set to false and we need to re generate the random index
					break;
				}
			}
		}
	}
	return theArr; // returning the unique array of random integers
}

/* Encode the string text into the red channel of image img.
 * Returns a new struct PPM, or NULL on error. */
struct PPM *encode(const char *text, const struct PPM *img)
{
	struct PPM *TheEncPpmFile = (struct PPM *)malloc(sizeof(struct PPM));
	TheEncPpmFile->format = img->format;
	TheEncPpmFile->height = img->height;
	TheEncPpmFile->width = img->width;
	TheEncPpmFile->max = img->max;
	TheEncPpmFile->rgbArr = img->rgbArr; // creating a copy of the old struct ppm

	int message_len = strlen(text); // setting the length of the actual message using strlen instead of sizeof
	int max_limit = TheEncPpmFile->width * TheEncPpmFile->height;

	if (message_len > max_limit)
	{
		printf("ERROR the text is too long");
		return NULL;
	}
	int *sortedRandArr = bubbleSort((randArr(message_len, 0, max_limit)), message_len); // gives us a sorted int array of random index values from 0 to max_limit so we can encode every char of the text at that index

	for (int i = 0; i < message_len; i++)
	{
		if (TheEncPpmFile->rgbArr[sortedRandArr[i]].red == text[i]) // in the condition when the current value at red pixel matches with ascii value of text[i]
		{
			for (int j = sortedRandArr[i] + 1; j < sortedRandArr[i + 1]; j++) // we iterate through current random index+1 till next random index
			// Linear search
			{
				while (TheEncPpmFile->rgbArr[sortedRandArr[j]].red != text[i]) // when the current red pixel index doesnt match the value of the character we shall input text[i]
				{
					TheEncPpmFile->rgbArr[sortedRandArr[j]].red = text[i];
					break;
				}
				if (j == sortedRandArr[i + 1]) // failure case
				{
					fprintf(stderr, "Error with adding the text, please start over. \n");
				}
			}
		}
		else
		{
			TheEncPpmFile->rgbArr[sortedRandArr[i]].red = text[i]; // setting the red pixel at index sortedRandArr[i] to text[i]
		}
	}
	return TheEncPpmFile;
}

/* Extract the string encoded in the red channel of newimg, by comparing it
 * with oldimg. The two images must have the same size.
 * Returns a new C string, or NULL on error. */
char *decode(const struct PPM *oldimg, const struct PPM *newimg)
{
	// check if sizes match
	if (oldimg->height != newimg->height || oldimg->width != newimg->width)
	{
		printf("ERROR on decoding, file sizes do not match! \n");
		return NULL;
	}

	int row_count = oldimg->height * oldimg->width;
	int char_count = 0;		// this is a variable that stores the number of character in the hidden text
	int altered_chars[500]; // max number of characters is 500
	for (int i = 0; i <= row_count; i++)
	{
		if (newimg->rgbArr[i].red != oldimg->rgbArr[i].red) // catching the differences in red pixel info
		{
			altered_chars[char_count] = newimg->rgbArr[i].red; // adding
			char_count++;
		}
	}
	char *text;						   // this is our text array
	text = (char *)malloc(char_count); // allocating memory of size charcount

	if (char_count == 0)
	{
		printf("no significant message found!\n");
		free(text);
		text = NULL;
		return text;
	}

	for (int j = 0; j < char_count; j++)
	{
		text[j] = altered_chars[j];
	}

	return text;
}

int main(int argc, char *argv[])
{
	/* Initialise the random number generator, using the time as the seed */
	srand(time(NULL));

	/* Parse command-line arguments */
	if (argc == 3 && strcmp(argv[1], "t") == 0)
	{
		/* Mode "t" - test PPM reading and writing */

		struct PPM *img = readPPM(argv[2]);
		showPPM(img);
	}
	else if (argc == 3 && strcmp(argv[1], "e") == 0)
	{
		/* Mode "e" - encode PPM */

		struct PPM *oldimg = readPPM(argv[2]);

		char *text; // 500 char is the text max capacity
		text = (char *)malloc(500);
		fprintf(stderr, "Please enter the text to be encoded: ");
		fgets(text, 500, stdin); // using fgets to take user input instead of scanf as scanf didnt include space character(ASCII character #32)
		// scanf("%s", text);

		struct PPM *newimg;

		newimg = encode(text, oldimg);
		fprintf(stderr, "\n The text has been successfully encoded!\n\n");
		showPPM(newimg);
		free(newimg);
	}
	else if (argc == 4 && strcmp(argv[1], "d") == 0)
	{
		/* Mode "d" - decode PPM */
		char *ogfilename;
		ogfilename = argv[2];

		struct PPM *oldimg;
		oldimg = readPPM(ogfilename);

		char *newfilename;
		newfilename = argv[3];

		struct PPM *newimg = readPPM(newfilename);

		char *message = decode(oldimg, newimg);

		if ((message) == NULL)
		{
			printf("message is NULL\n");
			exit(0);
		}
		printf("The decoded message is: ");
		for (int i = 0; i <= strlen(message); i++)
		{
			printf("%c", message[i]);
		}
		printf("\n");
	}
	else
	{
		fprintf(stderr, "Unrecognised or incomplete command line.\n");
		return 1;
	}

	return 0;
}
