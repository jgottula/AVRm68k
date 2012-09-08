/* AVRm68k [test programs]
 * (c) 2012 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details. */

/* test file for determining m68k assembly output from C source */

int square(int x)
{
	return x * x;
}

int main(void)
{
	return square(4);
}
