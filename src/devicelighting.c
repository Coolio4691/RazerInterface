#include "devicelighting.h"
#include "devicemanager.h"

#include <stdlib.h>
#include <stdio.h>

int device_lighting_init(char* device, struct lighting* lighting) {
    int code = devicemanager_get_device_matrix(device, &lighting->matrix);

    // exit if error code returned
    if(code < 0) return code;

    // allocate size of matrix to the colour array
    size_t totalSize = lighting->matrix.rows * lighting->matrix.cols;
    lighting->colourArray = calloc(sizeof(struct rgb), totalSize);

    // set all values to 0
    device_lighting_reset(lighting);

    // return code
    return code;
}


void device_lighting_reset(struct lighting* lighting) {
    // set all values to empty in array
    struct rgb empty;
    empty.red = 0;
    empty.green = 0;
    empty.blue = 0;

    for(size_t i = 0; i < lighting->matrix.rows * lighting->matrix.cols; i++) {
        lighting->colourArray[i] = empty;
    }
};


void device_lighting_set_key(struct lighting* lighting, int key, int r, int g, int b) {
    // set key at index to values
    lighting->colourArray[key].red = r;
    lighting->colourArray[key].green = g;
    lighting->colourArray[key].blue = b;
};

struct rgb device_lighting_get_key(struct lighting* lighting, int key) {
    // get value from key at index
    return lighting->colourArray[key];
};


int key_index_from_2D(struct lighting* lighting, int x, int y) {
    // index = cols * y + x
    return lighting->matrix.cols * y + x;
}

int* key_2D_from_index(struct lighting* lighting, int index) {
    // create position variable (uses malloc so have to free it where it is used)
    int* position = malloc(2 * sizeof(int));

    // x = index % cols
    position[0] = index % lighting->matrix.cols;
    // y = index / cols
    position[1] = index / lighting->matrix.cols;

    // return x = index % cols, y = index / cols
    return position;
}


void lighting_row_bytes(struct lighting* lighting, int row, unsigned char** bytes, int* bytesSize) {
    // allocate 3 + (column size * rgb(3))
    *bytes = malloc(3 + ((lighting->matrix.cols - 1) * 3));
    // set size to 0
    *bytesSize = 0;

    // set bytes at 0 to current row
    (*bytes)[(*bytesSize)++] = row;
    // set bytes at 1 to 0
    (*bytes)[(*bytesSize)++] = 0;
    // set bytes at 2 to the column amount
    (*bytes)[(*bytesSize)++] = lighting->matrix.cols - 1;

    for(int col = 0; col < lighting->matrix.cols; col++) {
        // get rgb values at col, row
        struct rgb rgb = device_lighting_get_key(lighting, key_index_from_2D(lighting, col, row));

        // append rgb values to the bytes array
        (*bytes)[(*bytesSize)++] = rgb.red;
        (*bytes)[(*bytesSize)++] = rgb.green;
        (*bytes)[(*bytesSize)++] = rgb.blue;
    }
}

void lighting_colour_bytes(struct lighting* lighting, unsigned char** bytes, int* bytesSize) {
    // create variable to store as a buffer for the byttes
    unsigned char* oldBytes;
    int oldBytesSize = 0;

    for(int row = 0; row < lighting->matrix.rows; row++) {
        // if row > 0 allocate oldbytes and free current bytes array
        if(row > 0) {
            oldBytes = malloc(*bytesSize);

            for(int i = 0; i < *bytesSize; i++) {
                oldBytes[i] = (*bytes)[i];
            }

            oldBytesSize = *bytesSize;
            
            free(*bytes);
        }

        // make variable to hold the bytes returned by lighting_row_bytes
        unsigned char* tempBytes;
        int tempBytesSize;
        
        lighting_row_bytes(lighting, row, &tempBytes, &tempBytesSize);
        // allocate new memory for bytes
        *bytes = malloc((*bytesSize) + tempBytesSize);

        // if row > 0 set the bytes back to its old bytes
        if(row > 0) {
            for(int i = 0; i < oldBytesSize; i++) {
                (*bytes)[i] = oldBytes[i];
            }

            // free oldbytes memory since not needed
            free(oldBytes);
        }

        // append the bytes to the top of bytes
        for(int i = 0; i < tempBytesSize; i++) {
            (*bytes)[(*bytesSize) + i] = tempBytes[i];
        }

        // add the rowbytessize to bytessize
        *bytesSize += tempBytesSize;

        // free tempbytes
        free(tempBytes);
    }
}