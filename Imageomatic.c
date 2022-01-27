/*
	Linguagens e Ambientes de Programação - Projeto de 2020/2021

	Imageomatic module body

 IDENTIFICAÇÃO DOS AUTORES -
	Aluno 1: 57369, Catarina Bento
	Aluno 2: 57482, Martim Gouveia

Comentarios:
	Todos os comandos foram implementados
*/

#include "Imageomatic.h"
#define MAX_LINE 1024
#define NUM_VALUES 256
#define WHITE "FFFFFF"
#define BLACK "000000"
typedef char Line[MAX_LINE];

/* Aux functions */

void stringToUpper(String s){
	int i;
	for( i = 0 ; s[i] != '\0' ; i++ ) {
		s[i] = toupper(s[i]);
	}
}

bool isHex(String cor){
	stringToUpper(cor);

	if(strlen(cor) != 6){
		return false;
	}
	for(int i = 0; i < 6; i++){
		char c = cor[i];
		if((!(c>='0' && c<='9')) && (!(c>='A' && c<='F'))){
			return false;
		}
	}
	return true;
}


/*** TYPE Pixel ***/

Pixel replacePixelGray(Pixel p){
	int b = pixelGrayAverage(p);
	return pixelGray(b);
}

Pixel replacePixelNegative(Pixel p){
	int r = MAX_COLOR - p.red;
	int g = MAX_COLOR - p.green;
	int b = MAX_COLOR - p.blue;

	return pixel(r, g, b);
}

/* Sets the color to the nearest multiple*/
int filterColor(int color, int values[], int nValues){
	int multiple = NUM_VALUES/nValues;
	for(int i = 0; i<nValues; i++){
		if((values[i] + multiple) > color){
			return values[i];
		}
	}
	return -1;
}

/* Calculates the multiple and applies filterColor to each of the 3 colors*/
Pixel filterPosterizePixel(Pixel p, int n){
	int nValues = 1<<n;	//2 to the power of n
	int values[nValues];

	int multiple = NUM_VALUES/nValues;

	int i = 0;
	for(int number = 0; number <= MAX_COLOR; number += multiple){
		values[i++] = number;
	}

	int red = filterColor(p.red, values, nValues);
	int green = filterColor(p.green, values, nValues);
	int blue = filterColor(p.blue, values, nValues);

	return pixel(red, green, blue);
}

Pixel blurPixel(Pixel p, Image img, Int2 n, Int2 i, int nivel){
	int red = 0;
	int green = 0;
	int blue = 0;

	int validPixel = 0;

	for(int y = (nivel * (-1)); y <= nivel; y++)
	for(int x = (nivel * (-1)); x <= nivel; x++){
		if(((i.x+x) >= 0 && (i.x+x) < n.x) && ((i.y+y) >= 0 && (i.y+y) < n.y)){
			red += img[i.x+x][i.y+y].red;
			green += img[i.x+x][i.y+y].green;
			blue += img[i.x+x][i.y+y].blue;
			validPixel++;
		}
	}
	return pixel((red/validPixel), (green/validPixel), (blue/validPixel));
}

Pixel maskPixel(Pixel p1, Pixel p2){
	double scaledRed = ((double)p2.red)/MAX_COLOR;
	double scaledGreen = ((double)p2.green)/MAX_COLOR;
	double scaledBlue = ((double)p2.blue)/MAX_COLOR;

	int r = (p1.red * scaledRed);
	int g = (p1.green * scaledGreen);
	int b = (p1.blue * scaledBlue);

	return pixel(r, g, b);
}

/*** TYPE Image ***/

void initialization(void){

}

Int2 imageCopy(Image img, Int2 n, Image res){
	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = img[i.x][i.y];
	}
	return n;
}

Int2 imageGrayscale(Image img, Int2 n, Image res){
	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = replacePixelGray(img[i.x][i.y]);
	}
	return n;
}

Int2 imageNegative(Image img, Int2 n, Image res){
	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = replacePixelNegative(img[i.x][i.y]);
	}
	return n;
}

Int2 imageHalf(Image img, Int2 n, Image res){
	Int2 i, j;
	for(i.y = 0, j.y = 0; i.y < n.y; i.y += 2, j.y++)
	for(i.x = 0, j.x = 0; i.x < n.x; i.x += 2, j.x++) {
		res[j.x][j.y] = img[i.x][i.y];
	}
	n.x = (n.x/2);
	n.y = (n.y/2);
	return n;
}

/* Aux imagePaint */
int colorValue(char a, char b){
	char ch[2] = {a, b};
	stringToUpper(ch);

	return strtol(ch, NULL, 16);
}

Int2 imagePaint(String cor, Int2 n, Image res){
	String corUpper;
	strcpy(corUpper, cor);
	stringToUpper(corUpper);

	Line l, s1, s2;
	char hex[MAX_LINE];
	bool isListed = false;

	FILE *fp = fopen(colorsFileName, "r");
	while((fgets(l, MAX_LINE, fp) != NULL) && !isListed){
		sscanf(l, "%s %s", s1, s2);
		if((strcmp(corUpper, s1) == 0) || (strcmp(cor, s2) == 0)){
			strcpy(hex, s1);
			isListed = true;
		}
	}

	if(!isListed){
		if(isHex(cor)){
			strcpy(hex, corUpper);
		}
		else{
			strcpy(hex, BLACK);
		}
	}

	int r = colorValue(hex[0], hex[1]);
	int g = colorValue(hex[2], hex[3]);
	int b = colorValue(hex[4], hex[5]);
	Pixel p = pixel(r, g, b);

	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = p;
	}
	return n;
}

Int2 imageRotation90(Image img, Int2 n, Image res){
	Int2 m = int2(n.y, n.x);

	Int2 i;
	for(i.y = 0; i.y < m.y; i.y++)
	for(i.x = 0; i.x < m.x; i.x++) {
		res[i.x][i.y] = img[i.y][((m.x-1)-i.x)];
	}

	return m;
}

Int2 imagePosterize(Image img, Int2 n, int factor, Image res){
	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = filterPosterizePixel(img[i.x][i.y], factor);
	}
	return n;
}

Int2 imageDroplet(Int2 n, Image res){
	Int2 c = int2Half(n);
	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		int gray = (0.7 * MAX_COLOR + 0.3 * sin(int2Distance(i, c) / 20.0) * MAX_COLOR);
		res[i.x][i.y] = pixelGray(gray);
	}
	return n;
}

Int2 imageBlur(Image img, Int2 n, int nivel, Image res){
	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = blurPixel(img[i.x][i.y], img, n, i, nivel);
	}
	return n;
}

Int2 imageMask(Image img1, Int2 n1, Image img2, Int2 n2, Image res){ // pre: int2Equals(n1, n2)
	Int2 i;
	for(i.y = 0; i.y < n1.y; i.y++)
	for(i.x = 0; i.x < n1.x; i.x++) {
		res[i.x][i.y] = maskPixel(img1[i.x][i.y], img2[i.x][i.y]);
	}
	return n1;
}

Int2 imageFunctionPlotting(DoubleFun fun, int scale, Int2 n, Image res){
	imagePaint(WHITE, n, res);

	for(int y = 0; y < n.y; y++){
		res[n.x/2][y] = pixelGray(0);
	}

	for(int x = 0; x < n.x; x++) {
		res[x][n.y/2] = pixelGray(0);
		int xGraph = x-(n.x/2);
		double yGraph = (scale*(fun((double)xGraph/scale)));
		res[x][(int)((n.y/2)-yGraph)] = pixelGray(0);
	}

	return n;
}

/* Aux imageOrderedDithering */
void matrixDithering(Image res, int indexSide, Byte indexMatrix[][indexSide], Int2 origin, Int2 n){
	Int2 limit = int2(origin.x+indexSide, origin.y+indexSide);
	for(int y = 0; y < indexSide; y++)
	for(int x = 0; x < indexSide; x++) {
		double value = pixelGrayAverage (res[origin.x + x][origin.y + y])/4.0;
		if(((origin.x + x) < n.x) && ((origin.y + y) < n.y)){
			if(value > indexMatrix[x][y]){
				res[origin.x + x][origin.y + y] = white;
			}
			else{
				res[origin.x + x][origin.y + y] = black;
			}
		}
	}
}

Int2 imageOrderedDithering(Image img, Int2 n, Image res){
	#define INDEX_SIDE  8
	Byte indexMatrix[INDEX_SIDE][INDEX_SIDE] = {
					{ 0, 32,  8, 40,  2, 34, 10, 42},
					{48, 16, 56, 24, 50, 18, 58, 26},
					{12, 44,  4, 36, 14, 46,  6, 28},
					{60, 28, 52, 20, 62, 30, 54, 22},
					{ 3, 35, 11, 43,  1, 33,  9, 41},
					{51, 19, 59, 27, 49, 17, 57, 25},
					{15, 47,  7, 39, 13, 45,  5, 37},
					{63, 31, 55, 23, 61, 29, 53, 21}
	};

	imageCopy(img, n, res);

	Int2 i;
	for(i.y = 0; i.y < n.y; i.y += INDEX_SIDE)
	for(i.x = 0; i.x < n.x; i.x += INDEX_SIDE) {
		matrixDithering(res, INDEX_SIDE, indexMatrix, i, n);
	}

	return n;
}

/* Aux secretPixel */
int secretColor(int color, char secret){
	int emtpyColor = (color & 0b11111100);
	return (emtpyColor | secret);
}

/* Aux imageSteganography */
Pixel secretPixel(char c, Pixel p){
	char sRed = (c & 0b110000) >> 4;
	char sGreen = (c & 0b001100) >> 2;
	char sBlue = (c & 0b000011);

	return 
		pixel(secretColor(p.red, sRed), secretColor(p.green, sGreen), secretColor(p.blue, sBlue));
}

/* Aux imageSteganography */
Pixel messagePixel(char c, Pixel p){
	char newC = toupper(c);
	if(newC>=' ' && newC<='_' && newC!='@'){
		if(newC > '@'){
			newC -= 64; //64 = 0x40
		}
		return secretPixel(newC, p);
	}
	else{
		return secretPixel('?', p);
	}
}

Int2 imageSteganography(Image img, Int2 n, String s, Image res){
	imageCopy(img, n, res);

	Int2 i;
	int j = 0;
	for(i.y = 0; (i.y < n.y) && (j < strlen(s)); i.y++)
	for(i.x = 0; (i.x < n.x) && (j < strlen(s)); i.x++, j++) {
		res[i.x][i.y] = messagePixel(s[j], img[i.x][i.y]);
	}

	//Calculate pixel to place \0
	i.x--;
	i.y--;

	if((i.x < n.x) && (i.y < n.y)){
		if((i.x != n.x-1) && (i.y != n.y-1)){
			if(i.x < (n.x-1)){
				i.x++;
			}
			else{
				i.y++;
				i.x = 0;
			}
		}
	}
	else{
		i.x = n.x-1;
		i.y = n.y-1;
	}

	res[i.x][i.y] = secretPixel('@'-64, img[i.x][i.y]);;

	return n;
}


void imageTests(void)
{
	static Image img, img2, res;
	Int2 n;

	// Q
	n = imageLoad("img/frutos.png", img);
	n = imageGrayscale(img, n, res);
	imageStore("img/cinzento.png", res, n);

	// N
	n = imageLoad("img/frutos.png", img);
	n = imageNegative(img, n, res);
	imageStore("img/negativo.png", res, n);

	// H
	n = imageLoad("img/frutos.png", img);
	n = imageHalf(img, n, res);
	imageStore("img/metade.png", res, n);

	// P
	n = int2(512, 512);
	n = imagePaint("green", n, res);
	imageStore("img/pintar.png", res, n);

	// R
	n = imageLoad("img/frutos.png", img);
	n = imageRotation90(img, n, res);
	imageStore("img/rotacao_90.png", res, n);

	// O
	n = imageLoad("img/frutos.png", img);
	n = imagePosterize(img, n, 3, res);
	imageStore("img/poster.png", res, n);

	// G
	n = int2(512, 512);
	n = imageDroplet(n, res);
	imageStore("img/goticula.png", res, n);

	// D
	n = imageLoad("img/frutos.png", img);
	n = imageBlur(img, n, 5, res);
	imageStore("img/desfocado.png", res, n);

	// M
	n = imageLoad("img/frutos.png", img);
	n = imageDroplet(n, img2);
	n = imageMask(img, n, img2, n, res);
	imageStore("img/mascarar.png", res, n);

	// F
	n = int2(512, 512);
	n = imageFunctionPlotting(sin, 50, n, res);
	imageStore("img/funcao.png", res, n);

	// T
	n = imageLoad("img/frutos.png", img);
	n = imageOrderedDithering(img, n, res);
	imageStore("img/matizacao.png", res, n);

	// E
	n = imageLoad("img/frutos.png", img);
	n = imageSteganography(img, n, "atacamos ao amanhecer", res);
	imageStore("img/esteganografia.png", res, n);
}
