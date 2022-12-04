//The original SDBP implementation has a very interesting hashing method, listed below
// hash three numbers into one
unsigned int mix (unsigned int a, unsigned int b, unsigned int c) {
	a=a-b;  a=a-c;  a=a^(c >> 13);
	b=b-c;  b=b-a;  b=b^(a << 8);
	c=c-a;  c=c-b;  c=c^(b >> 13);
	return c;
}

// first hash function

unsigned int f1 (unsigned int x) {
	return mix (0xfeedface, 0xdeadb10c, x);
}

// second hash function

unsigned int f2 (unsigned int x) {
	return mix (0xc001d00d, 0xfade2b1c, x);
}

// generalized hash function

unsigned int fi (unsigned int x, int i) {
	return f1 (x) + (f2 (x) >> i);
}