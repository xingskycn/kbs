/*

cipher.c

Author: Tatu Ylonen <ylo@cs.hut.fi>

Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
                   All rights reserved

Created: Wed Apr 19 17:41:39 1995 ylo

*/

#include "sys/types.h"
#include "./md5.h"
#include "includes.h"
#include "ssh.h"
#include "cipher.h"

/* Names of all encryption algorithms.  These must match the numbers defined
   int cipher.h. */
static char *cipher_names[] = { "none", "idea", "des", "3des", "used to be tss", "arcfour", "blowfish",
    "reserved"
};

/* Returns a bit mask indicating which ciphers are supported by this
   implementation.  The bit mask has the corresponding bit set of each
   supported cipher. */

unsigned int cipher_mask(void)
{
    unsigned int mask = 0;

#ifdef WITH_NONE
    mask |= 1 << SSH_CIPHER_NONE;
#endif                          /* WITH_NONE */

#ifdef WITH_IDEA
    mask |= 1 << SSH_CIPHER_IDEA;
#endif                          /* WITH_IDEA */

#ifdef WITH_DES
    mask |= 1 << SSH_CIPHER_DES;
#endif                          /* WITH_DES */

    mask |= 1 << SSH_CIPHER_3DES;

#ifdef WITH_ARCFOUR
    mask |= 1 << SSH_CIPHER_ARCFOUR;
#endif                          /* WITH_ARCFOUR */

#ifdef WITH_BLOWFISH
    mask |= 1 << SSH_CIPHER_BLOWFISH;
#endif                          /* WITH_BLOWFISH */
    return mask;
}

/* Returns the name of the cipher. */

const char *cipher_name(int cipher)
{
    if (cipher < 0 || cipher >= sizeof(cipher_names) / sizeof(cipher_names[0]))
        fatal("cipher_name: bad cipher number: %d", cipher);
    return cipher_names[cipher];
}

/* Parses the name of the cipher.  Returns the number of the corresponding
   cipher, or -1 on error. */

int cipher_number(const char *name)
{
    int i;
    static int warning_given = 0;

    /* Recognize other nam for backward compatibility. */
    if (name[0] == 'r' && name[1] == 'c' && name[2] == '4' && name[3] == '\0') {
#if defined(WITH_ARCFOUR) || !defined(WITH_BLOWFISH)
        return SSH_CIPHER_ARCFOUR;
#else
        if (!warning_given)
            log_msg("Arcfour cipher is disabled in this host, using blowfish cipher instead");
        warning_given = 1;
        return SSH_CIPHER_BLOWFISH;
#endif
    }

    for (i = 0; i < sizeof(cipher_names) / sizeof(cipher_names[0]); i++)
        if (strcmp(cipher_names[i], name) == 0) {
#if !defined(WITH_ARCFOUR) && defined(WITH_BLOWFISH)
            if (i == SSH_CIPHER_ARCFOUR) {
                if (!warning_given)
                    log_msg("Arcfour cipher is disabled in this host, using blowfish cipher instead");
                warning_given = 1;
                return SSH_CIPHER_BLOWFISH;
            }
#endif
            return i;
        }
    return -1;
}

/* Selects the cipher, and keys if by computing the MD5 checksum of the
   passphrase and using the resulting 16 bytes as the key. */

void cipher_set_key_string(CipherContext * context, int cipher, const char *passphrase, int for_encryption)
{
    struct MD5Context md;
    unsigned char digest[16];

    MD5Init(&md);
    MD5Update(&md, (const unsigned char *) passphrase, strlen(passphrase));
    MD5Final(digest, &md);

    cipher_set_key(context, cipher, digest, 16, for_encryption);

    memset(digest, 0, sizeof(digest));
    memset(&md, 0, sizeof(md));
}

/* Selects the cipher to use and sets the key. */

void cipher_set_key(CipherContext * context, int cipher, const unsigned char *key, int keylen, int for_encryption)
{
    unsigned char padded[32];

    /* Clear the context to remove any traces of old keys. */
    memset(context, 0, sizeof(*context));

    /* Set cipher type. */
    context->type = cipher;

    /* Get 32 bytes of key data.  Pad if necessary.  (So that code below does
       not need to worry about key size). */
    memset(padded, 0, sizeof(padded));
    memcpy(padded, key, keylen < sizeof(padded) ? keylen : sizeof(padded));

    /* Initialize the initialization vector. */
    switch (cipher) {
    case SSH_CIPHER_NONE:
        break;

#ifdef WITH_IDEA
    case SSH_CIPHER_IDEA:
        if (keylen < 16)
            error("Key length %d is insufficient for IDEA.", keylen);
        idea_set_key(&context->u.idea.key, padded);
        memset(context->u.idea.iv, 0, sizeof(context->u.idea.iv));
        break;
#endif                          /* WITH_IDEA */

#ifdef WITH_DES
    case SSH_CIPHER_DES:
        /* Note: the least significant bit of each byte of key is parity, 
           and must be ignored by the implementation.  8 bytes of key are
           used. */
        if (keylen < 8)
            error("Key length %d is insufficient for DES.", keylen);
        des_set_key(padded, &context->u.des.key);
        memset(context->u.des.iv, 0, sizeof(context->u.des.iv));
        break;
#endif                          /* WITH_DES */

    case SSH_CIPHER_3DES:
        /* Note: the least significant bit of each byte of key is parity, 
           and must be ignored by the implementation.  16 bytes of key are
           used (first and last keys are the same). */
        if (keylen < 16)
            error("Key length %d is insufficient for 3DES.", keylen);
        des_set_key(padded, &context->u.des3.key1);
        des_set_key(padded + 8, &context->u.des3.key2);
        if (keylen <= 16)
            des_set_key(padded, &context->u.des3.key3);
        else
            des_set_key(padded + 16, &context->u.des3.key3);
        memset(context->u.des3.iv1, 0, sizeof(context->u.des3.iv1));
        memset(context->u.des3.iv2, 0, sizeof(context->u.des3.iv2));
        memset(context->u.des3.iv3, 0, sizeof(context->u.des3.iv3));
        break;

#ifdef WITH_ARCFOUR
    case SSH_CIPHER_ARCFOUR:
        arcfour_init(&context->u.arcfour, key, keylen);
        break;
#endif                          /* WITH_ARCFOUR */

#ifdef WITH_BLOWFISH
    case SSH_CIPHER_BLOWFISH:
        if (keylen < 8)
            error("Key length %d is insufficient for Blowfish", keylen);
        blowfish_set_key(&context->u.blowfish, key, keylen, for_encryption);
        break;
#endif                          /* WITH_BLOWFISH */
    default:
        fatal("cipher_set_key: unknown cipher: %d", cipher);
    }
    memset(padded, 0, sizeof(padded));
}

/* Encrypts data using the cipher. */

void cipher_encrypt(CipherContext * context, unsigned char *dest, const unsigned char *src, unsigned int len)
{
    switch (context->type) {
    case SSH_CIPHER_NONE:
        memcpy(dest, src, len);
        break;

#ifdef WITH_IDEA
    case SSH_CIPHER_IDEA:
        idea_cfb_encrypt(&context->u.idea.key, context->u.idea.iv, dest, src, len);
        break;
#endif                          /* WITH_IDEA */

#ifdef WITH_DES
    case SSH_CIPHER_DES:
        des_cbc_encrypt(&context->u.des.key, context->u.des.iv, dest, src, len);
        break;
#endif                          /* WITH_DES */

    case SSH_CIPHER_3DES:
        des_3cbc_encrypt(&context->u.des3.key1, context->u.des3.iv1, &context->u.des3.key2, context->u.des3.iv2, &context->u.des3.key3, context->u.des3.iv3, dest, src, len);
        break;

#ifdef WITH_ARCFOUR
    case SSH_CIPHER_ARCFOUR:
        arcfour_encrypt(&context->u.arcfour, dest, src, len);
        break;
#endif                          /* WITH_ARCFOUR */

#ifdef WITH_BLOWFISH
    case SSH_CIPHER_BLOWFISH:
        blowfish_cbc_encrypt(&context->u.blowfish, dest, src, len);
        break;
#endif                          /* WITH_BLOWFISH */

    default:
        fatal("cipher_encrypt: unknown cipher: %d", context->type);
    }
}

/* Decrypts data using the cipher. */

void cipher_decrypt(CipherContext * context, unsigned char *dest, const unsigned char *src, unsigned int len)
{
    switch (context->type) {
    case SSH_CIPHER_NONE:
        memcpy(dest, src, len);
        break;

#ifdef WITH_IDEA
    case SSH_CIPHER_IDEA:
        idea_cfb_decrypt(&context->u.idea.key, context->u.idea.iv, dest, src, len);
        break;
#endif                          /* WITH_IDEA */

#ifdef WITH_DES
    case SSH_CIPHER_DES:
        des_cbc_decrypt(&context->u.des.key, context->u.des.iv, dest, src, len);
        break;
#endif                          /* WITH_DES */

    case SSH_CIPHER_3DES:
        des_3cbc_decrypt(&context->u.des3.key1, context->u.des3.iv1, &context->u.des3.key2, context->u.des3.iv2, &context->u.des3.key3, context->u.des3.iv3, dest, src, len);
        break;

#ifdef WITH_ARCFOUR
    case SSH_CIPHER_ARCFOUR:
        arcfour_decrypt(&context->u.arcfour, dest, src, len);
        break;
#endif                          /* WITH_ARCFOUR */

#ifdef WITH_BLOWFISH
    case SSH_CIPHER_BLOWFISH:
        blowfish_cbc_decrypt(&context->u.blowfish, dest, src, len);
        break;
#endif                          /* WITH_BLOWFISH */

    default:
        fatal("cipher_decrypt: unknown cipher: %d", context->type);
    }
}
