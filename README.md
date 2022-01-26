RSA encoding/decoding implementation

Command line usage:

./rsa -h

Using as library:

Keys_t k
genkeys(&keys)
encodeStream(keys, "filename")
decodeStream(keys, "filename")


