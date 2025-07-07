#include "math_functions.h"

#define EPSILON 1e-10

// Function to compute e^x using Taylor series expansion
double exponents(double x) {
    double sum = 1.0;  // Initialize sum with the first term of the series
    double term = 1.0; // To hold each term in the series
    int n = 1;

    while (term > EPSILON) {
        term *= x / n;  // Compute the next term
        sum += term;   // Add the term to the sum
        ++n;
    }

    return sum;
}

// Function to compute log(x) using a combination of Taylor series and transformations
double logrithm(double x) {
    if (x <= 0) {
        return -1;  // Logarithm of zero or negative number is not defined
    }

    if (x == 1.0) {
        return 0.0; // log(1) = 0
    }

    if (x == 2.0) {
        return 1.0; // log(2) = 1
    }

    int integer_part;
    
    // Extract the integer part
    integer_part = (int)x;
    
    // Check if the input has .0 after the decimal point
    if ((x - integer_part) == 0.0) {
        double count = 0;
        int temp = integer_part;
        
        // Determine how many times the integer part is divisible by 2
        if(temp % 2 == 0){
            while(temp != 0){
                temp /= 2;
                count++;
            }
            return count - 1;
        }

    }

    // Shift x to be within the range (1, 2) for better approximation
    double shiftedX = x;
    int shiftCount = 0;
    
    while (shiftedX > 2.0) {
        shiftedX = (double) shiftedX / 2.0;
        shiftCount++;
    }
    while (shiftedX < 1.0) {
        shiftedX *= 2.0;
        shiftCount--;
    }

    // Approximate log(shiftedX) using the Taylor series for log(1 + y)
    double y = shiftedX - 1.0;
    double term = y;
    double result = 0.0;
    int n = 1;

    while (term > EPSILON || term < -EPSILON) {
        result += term / n;
        term *= -y;  // Alternate sign and multiply by y
        ++n;
    }

    // Adjust the result for the shifts we made earlier
    result += shiftCount * logrithm(2.0);

    return result;
}

// modfloat function: Splits a floating-point number into its integer and fractional parts
double modfloat(double value, double *intpart) {
    *intpart = (double)((int)value);  // Extract the integer part
    return value - *intpart;          // Return the fractional part
}

// pow function: Computes base raised to the power of exponent
double powers(double base, double exponent) {
    if (exponent == 0) return 1.0;    // base^0 = 1
    if (base == 0) return 0.0;        // 0^exponent = 0

    double result = 1.0;
    int exponentInt = (int)exponent;
    double fractionalPart = exponent - exponentInt;

    // Compute the integer part of the exponentiation
    for (int i = 0; i < exponentInt; ++i) {
        result *= base;
    }

    // Handle fractional exponent (approximation)
    if (fractionalPart > 0) {
        result *= exponents(fractionalPart * logrithm(base));  // Approximation using exp and log
    }

    return result;
}

// sqrt function: Computes the square root of a number using Newton's method
double squareroot(double value) {
    if (value < 0) return -1;  // Square root of negative number is not handled

    double guess = value;
    double epsilon = 1e-10;
    
    while ((guess * guess - value) > epsilon || (guess * guess - value) < -epsilon) {
        guess = (guess + value / guess) / 2;
    }

    return guess;
}

// cbrt function: Computes the cube root of a number
double cuberoot(double value) {
    if (value == 0) return 0;

    double guess = value;
    double epsilon = 1e-10;

    while ((guess * guess * guess - value) > epsilon || (guess * guess * guess - value) < -epsilon) {
        guess = (2 * guess + value / (guess * guess)) / 3;
    }

    return guess;
}
