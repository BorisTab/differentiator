//
// Created by boristab on 13.11.2019.
//

#include <assert.h>
#include <iostream>

struct lineIndex {
    char *startIndex;
    char *endIndex;
};

/*! get file size in bytes
 *
 * @param[in] inPath path to input file
 *
 * @return file size in bytes
 */
int getFileSize(const char *inPath) {
    assert(inPath != nullptr);

    FILE *myFile = fopen(inPath, "r");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }
    fseek(myFile, 0, SEEK_END);
    size_t textSize = ftell(myFile);
    fclose(myFile);
    return textSize;
}

/*! read text from file
 *
 * @param[in] inPath path to input file
 * @param[out] *text pointer to buffer for text
 * @param[in] textSize file size in bytes
 *
 * @return 0 if read success, errno if read fall
 */
int readFile(const char inPath[], char *text, size_t textSize) {
    assert(inPath != "");
    assert(text != nullptr);

    FILE *myFile = fopen(inPath, "r");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }
    fread(text, 1, textSize, myFile);
    fclose(myFile);
    return 0;
}

/*! strings compare
 *
 * @param[in] str1 Pointer to str1
 * @param[in] str2 Pointer to str2
 * @return negative value if string2 > string1, positive value if string1 > string2, 0 if string1 = string2
 */
int strCmp(const char *str1, const char *str2) {
    assert(str1 != nullptr);
    assert(str2 != nullptr);

    while (*str1 != '\0' || *str2 != '\0') {
        while (!isalpha(*str1)) str1++;
        while (!isalpha(*str2)) str2++;

        int ch1 = tolower(*str1);
        int ch2 = tolower(*str2);

        if (ch1 - ch2 != 0) return ch1 - ch2;
        if (*(str1 + 1) == '\0' && *(str2 + 1) == '\0') return 0;
        str1++;
        str2++;
    }
}

/*! String comparison
 *
 * @param[in] *string1 pointer to struct with pointers to string
 * @param[in] *string2 pointer to struct with pointers to string
 *
 * @return negative value if string2 > string1, positive value if string1 > string2, 0 if string1 = string2
 */
int strCmpForStruct(const void *string1, const void *string2) {
    assert(string1 != nullptr);
    assert(string2 != nullptr);

    char *str1 = (*((lineIndex *) string1)).startIndex;
    char *str2 = (*((lineIndex *) string2)).startIndex;

    return strCmp(str1, str2);
}

/*! back compare for strings
 *
 * @param[in] str1Start Pointer to first letter in str1
 * @param[in] str1End Pointer to last letter in str1
 * @param[in] str2Start Pointer to first letter in str2
 * @param[in] str2End Pointer to last letter in str2
 * @return n > 0 negative value if string2 > string1, positive value if string1 > string2, 0 if string1 = string2
 */
int strBackCmp(const char *str1Start, const char *str1End, const char *str2Start, const char *str2End) {
    while (str1End != str1Start || str2End != str2Start) {
        while (!isalpha(*str1End)) str1End--;
        while (!isalpha(*str2End)) str2End--;

        int ch1 = tolower(*str1End);
        int ch2 = tolower(*str2End);

        if (ch1 - ch2 != 0) return ch1 - ch2;
        if (str1End - 1 == str1Start && str2End - 1 == str2Start) return 0;

        str1End--;
        str2End--;
    }
}

/*! String back comparison
 *
 * @param[in] *string1 pointer to struct with pointers to string
 * @param[in] *string2 pointer to struct with pointers to string
 *
 * @return negative value if string2 > string1, positive value if string1 > string2, 0 if string1 = string2
 */
int strBackCmpForStruct(const void *string1, const void *string2) {
    assert(string1 != nullptr);
    assert(string2 != nullptr);

    char *str1End = (*((lineIndex *) string1)).endIndex;
    char *str2End = (*((lineIndex *) string2)).endIndex;

    char *str1Start = (*((lineIndex *) string1)).startIndex;
    char *str2Start = (*((lineIndex *) string2)).startIndex;

    return strBackCmp(str1Start, str1End, str2Start, str2End);
}

/*! Count rows in string
 *
 * @param[in] str input string
 * @param[in] textSize size of input string in bytes
 * @param[in] element element to compare
 *
 * @return number of rows
 */
int nRows(const char *str, size_t textSize, char element) {
    assert(str != nullptr);

    int nRows = 1;
    for (size_t i = 0; i < textSize; i++)
        if (str[i] == element) {
            nRows++;
        }
    return nRows;
}

/*! write text to file
 *
 * @param[in] outPath path to output file
 * @param[out] *text pointer to struct with pointers to strings
 * @param[in] textSize file size in bytes
 *
 * @return 0 if write success, errno if write fall
 */
int writeFile(const char outPath[], lineIndex *text, size_t rows) {
    assert(outPath != nullptr);
    assert(text != nullptr);

    FILE *myFile = fopen(outPath, "a");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }

    for (size_t i = 0; i < rows; i++) {
        fprintf(myFile, "%s\n", text[i].startIndex);
    }
    fprintf(myFile, "\n\n");
    fclose(myFile);
    return 0;
}

/*! write text to file
 *
 * @param[in] outPath path to output file
 * @param[out] *text pointer to struct with pointers to strings
 * @param[in] textSize file size in bytes
 *
 * @return 0 if write success, 1 if write fall
 */
int writeFile(const char outPath[], char **text, size_t rows) {
    assert(outPath != nullptr);
    assert(text != nullptr);

    FILE *myFile = fopen(outPath, "a");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }

    for (size_t i = 0; i < rows; i++) {
        fprintf(myFile, "%s\n", text[i]);
    }
    fprintf(myFile, "\n\n");
    fclose(myFile);
    return 0;
}

/*! fill index by pointers on rows
 *
 * @param index
 * @param text
 * @param textSize
 */
void fillIndex(lineIndex *index, char *text, size_t textSize) {
    assert(index != nullptr);
    assert(text != nullptr);

    index[0].startIndex = text;
    int lines = 1;
    for (size_t i = 0; i < textSize; i++) {
        if (text[i] == '\n') {
            index[lines - 1].endIndex = &text[i] - 1;
            index[lines].startIndex = &text[i] + 1;

            lines++;

            text[i] = '\0';
        }
    }
    text[textSize] = '\0';
    index[lines - 1].endIndex = &text[textSize] - 1;
}

/*! get copy of index
 *
 * @param[in] index Pointer in index for coping
 * @param[out] indexCopy Pointer on copied index
 * @param[in] rows number of rows
 */
void getIndexCopy(lineIndex *index, lineIndex *indexCopy, size_t rows) {
    assert(index != nullptr);
    assert(indexCopy != nullptr);

    for (size_t i = 0; i < rows; i++) {
        indexCopy[i].startIndex = index[i].startIndex;
        indexCopy[i].endIndex = index[i].endIndex;
    }
}

/*! read text from file
 *
 * @param[in] inPath path to read file
 * @param[out] text Pointer to buffer for read text
 * @param[out] index Pointer to array of structures with pointers on string start and string end
 * @param[out] textSize text size in bytes
 * @param[out] rows number of lines in text
 */
lineIndex *readTextFromFile(const char inPath[], char *text, size_t *textSize, size_t *rows) {
    assert(inPath != nullptr);

    *textSize = getFileSize(inPath);

    text = (char *) calloc(*textSize + 1, sizeof(char));

    if (readFile(inPath, text, *textSize)) exit(errno);
    *rows = nRows(text, *textSize, '\n');

    auto *index = (lineIndex *) calloc(*rows, sizeof(lineIndex));
    fillIndex(index, text, *textSize);

    return index;
}