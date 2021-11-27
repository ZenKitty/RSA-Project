// CSCI 411 Personal Project
// April 7th, 2021
// Code by Duncan Hendrickson

#include <iostream>
#include <algorithm>
#include <math.h>
#include <cmath>
#include <vector>
#include <string.h>

using namespace std;

/**********************STRUCTS**********************/
/****************************************************
 * Struct to store private exponent d and modulus n *
 ****************************************************/
struct PrivateKey
{
    // Modulus
    int n;
    // Private Exponent
    int d;
};

/****************************************************
 * Struct to store public exponent e and modulus n  *
 ****************************************************/
struct PublicKey
{
    // Modulus
    int n;
    // Public Exponent
    int e;
};

/********************************************************
 * Struct holding one public key and one private key    *
 ********************************************************/
struct Keypair
{
    PublicKey pub;
    PrivateKey pri;
};

/**********************FUNCTIONS**********************/
/****************************************************
 * A function to check if given a number is prime   *
 * Returns a bool                                   *
 * p - int - number to check primality              *
 ***************************************************/
bool isPrime(int p)
{
    bool prime = true;
    if (p <= 0 || p == 1)
    {
        prime = false;
    }
    else
    {
        for (int i = 2; i < p / 2; i++)
        {
            if (p % i == 0)
            {
                prime = false;
                break;
            }
        }
    }
    return prime;
}

/************************************************************************************
 * A function that finds the modular inverse via the Extended Euclidian Algorithm   *
 * a - int - the number you are finding the modular inverse of                      *
 * m - int - the modulus that you are using to find the inverse                     *
 ***********************************************************************************/
int modInverse(int a, int m)
{
    int m0 = m;
    int y = 0, x = 1;

    if (m == 1)
        return 0;

    while (a > 1)
    {
        // q is quotient
        int q = a / m;
        int t = m;

        // m is remainder now, process same as Euclid's algorithm
        m = a % m;
        a = t;
        t = y;

        // Update y and x
        y = x - q * y;
        x = t;
    }

    // Make x positive
    if (x < 0)
        x += m0;

    return x;
}

/************************************************************************************
 * A function to efficiently calculate a number to a power, then calculate the mod  *
 * Represented by equation result = base^exponent % modulus                         *
 * Don't have to calculate full value of base^exponent because of the fact that     *
 * a=bc mod n=(b mod n)⋅(c mod n)mod n                                              *
 * Returns a final result int                                                       *
 * base - int - the number that will be raised to some power                        *
 * exponent - int - the power that the base will be raised to                       *
 * modulus - int - the number that the result will be % by                          *
 ***********************************************************************************/
int powerMod(int base, int exponent, int modulus)
{
    if (modulus == 1)
        return 0;
    int result = 1;
    base = base % modulus;
    while (exponent > 0)
    {
        if (exponent % 2 == 1) //odd number
            result = (result * base) % modulus;
        exponent = exponent >> 1; //divide by 2
        base = (base * base) % modulus;
    }
    return result;
}

/************************************************************************************
 * Function to generate private and public keys based on user input                 *
 * keys - Keypair - passed by reference to allow modification                       *
 * Takes in user input for primes p and q, calculates n and phi, asks for input for *
 * the public exponent then calculates private exponent and stores in keypair       *
 ***********************************************************************************/
void generateKeys(Keypair &keys)
{
    int p = 0, q = 0; // Primes
    int n;            // p*q
    int phi;          // (p-1)*(q-1)
    int e, d;         // public and private exponent
    cout << "Enter two distinct prime numbers: ";
    while (true)
    {
        // read in user input for p and q
        cin >> p;
        cin.ignore();
        cin >> q;
        cin.ignore();
        if (p == q)
        {
            cout << "Those are the same number, try again: " << endl;
        }
        else if (p < 3 || q < 3)
        {
            cout << "At least one of those is less than 3, try again: " << endl;
            cout << p << " " << q << endl;
        }
        // ensures they are prime, calculates n and phi
        else if (isPrime(p) && isPrime(q))
        {
            n = p * q;
            phi = ((p - 1) * (q - 1));
            cout << "n = " << n << endl
                 << "phi = " << phi << endl;
            break;
        }
        else
        {
            cout << "One of those is not prime, try again: " << endl;
        }
    }
    // Get user input for public exponent
    cout << "Choose an e that does not share any factors with phi (Try more primes): ";
    while (true)
    {
        cin >> e;
        if (!(__algo_gcd(e, phi) == 1))
        {
            cout << "That doesn't work, try again please: " << endl;
        }
        // Calculate d using modular inversion
        else
        {
            cout << "Success, Calculating d..." << endl;
            d = modInverse(e, phi);
            cout << "d = " << d << endl;
            break;
        }
    }

    // Store Private and Public Key info
    keys.pri.d = d;
    keys.pri.n = n;
    keys.pub.e = e;
    keys.pub.n = n;

    cout << "Public Key = (n, e) = (" << n << ", " << e << ")" << endl;
    cout << "Private Key = (n, d) = (" << n << ", " << d << ")" << endl;
}

/************************************************************************
 * Function to take in user input and encrypt using c = m^e % n         *
 * pub - PublicKey - stores n and e for this keypair for encryption     *
 * returns vector of ints that represents each encrypted letter         *
 ***********************************************************************/
vector<int> encrypt(PublicKey pub)
{
    // Read in text to encrypt
    cout << "Please enter your text to be encrypted: ";
    string text;
    cin.ignore();
    getline(cin, text);

    int len = text.length();
    vector<int> cypher(len);
    // For each character, encrypt using c = m^e % n
    for (int i = 0; i < len; i++)
    {
        int ascii = static_cast<int>(text[i]);
        cypher[i] = powerMod(ascii, pub.e, pub.n);
    }

    // Print encrypted data for aesthetic's sake
    cout << endl
         << "Encrypted message in hex:" << endl;
    for (int i = 0; i < len; i++)
    {
        cout << hex << cypher[i];
    }
    cout << endl
         << endl;

    // Return encrypted data
    return cypher;
}

/************************************************************************
 * Function to take in ciphertext input and decrypt using m = c^d % n   *
 * pri - PrivateKey - stores n and d for this keypair for decryption    *
 * cypher - vector<int> - represents each encrypted letter              *
 ***********************************************************************/
void decrypt(PrivateKey pri, vector<int> cypher)
{
    int decrypt[cypher.size()];
    cout << "Your message decrypted: " << endl;
    // For each character, decrypt using m = c^d % n
    for (int i = 0; i < cypher.size(); i++)
    {
        decrypt[i] = powerMod(cypher[i], pri.d, pri.n);
        cout << static_cast<char>(decrypt[i]);
    }
    cout << endl;
}

/**********************MAIN**********************/
int main()
{
    // Create an empty keypair
    Keypair keys;
    // Generate keys with user input
    generateKeys(keys);
    // Encrypt user inputed text with public key
    vector<int> secret = encrypt(keys.pub);
    // Immediately decrypt
    decrypt(keys.pri, secret);

    return 0;
}