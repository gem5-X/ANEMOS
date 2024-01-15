/* 
 * 
 * Function for obtaining the rounded-up log2 of an integer.
 *
 */

#ifndef LOG2_H
#define LOG2_H

uint log2c(uint n) {
    uint m = 0;
    uint p = 1;

    while (p < n) {
        m = m + 1;
        p = p * 2;
    }
    return m;
}

#endif
