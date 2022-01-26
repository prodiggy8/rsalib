RSA encoding/decoding implementation

This was written for educational purposes. This implementation is vulnerable to
countless cryptanalysis attacks, and is not considered secure nor intended for
use in production systems.

by and for Prodiggy
== Cybersec ==


Command line usage:

./rsa -h

Using as library:

Keys_t k
genkeys(&keys)
encodeStream(keys, "filename")
decodeStream(keys, "filename")


