#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS	480
#define COLS	640

#define sqr(x)	((x)*(x))

void clear( unsigned char image[][COLS] );
void header( int row, int col, unsigned char head[32] );

int main( int argc, char** argv )
{
	int				i, j, sgm_max, temp[ROWS][COLS];
	int				dedx, dedy, sgm, max1, max2, max3, theta1, theta2, theta3, rho1, rho2, rho3;
	int				sgm_threshold, hough_threshold, voting[180][1600];
	double 		rho, theta, radian, radian1, radian2, radian3;
	FILE*			fp;
	unsigned char	image[ROWS][COLS], head[32];
	char			filename[50];
	double  	pi = 3.14159;
	
	strcpy ( filename, "image.raw" );
	memset ( voting, 0, sizeof(int) * 180 * 1600 );
	header ( ROWS, COLS, head );

	if (!( fp = fopen( filename, "rb" ) ))
	{
		fprintf( stderr, "error: couldn't open %s\n", argv[1] );
		exit(1);
	}

	for ( i = 0 ; i < ROWS ; i++ )
		if (!( COLS == fread( image[i], sizeof(char), COLS, fp ) ))
		{
			fprintf( stderr, "error: couldn't read %s\n", argv[1] );
			exit(1);
		}
	fclose(fp);
	
	sgm_max = 0; 
	hough_threshold = 180;
	
	//Calculate Gx and Gy in order to calculate sgm
	for ( i = ROWS-2 ; i > 0 ; i-- ){
		for ( j = COLS-2 ; j > 0 ; j-- )
		{
			dedx =
					  -image[i-1][j-1] +   image[i-1][j+1] +
					-2*image[ i ][j-1] + 2*image[ i ][j+1] +
					  -image[i+1][j-1] +   image[i+1][j+1] ;

			dedy =
				-image[i-1][j-1] - 2*image[i-1][ j ] - image[i-1][j+1] +
				 image[i+1][j-1] + 2*image[i+1][ j ] + image[i+1][j+1] ;

			temp[i][j] = sqr(dedx) + sqr(dedy);
			
			//finding max sgm value
			if (temp[i][j] > sgm_max){
					sgm_max = temp[i][j];
			}
		}
	}
	// normalizing
	for( i = 0; i< ROWS; i++){
		for( j = 0; j< COLS; j++){
			if (i == 0 || i == ROWS-1 || j == 0 || j == COLS-1){
				image[i][j] = 0;
			}
			else{
				image[i][j] = ((double) temp[i][j])/sgm_max * 255;
			}
		}
	}
	
	
	/* Write SGM to a new image */
	strcpy( filename, "image" );
	if (!( fp = fopen( strcat( filename, "-sgm.ras" ), "wb" ) ))
	{
	  fprintf( stderr, "error: could not open %s\n", filename );
	  exit( 1 );
	}
	fwrite( head, 4, 8, fp );
	for ( i = 0 ; i < ROWS ; i++ ) fwrite( image[i], 1, COLS, fp );

	fclose( fp );
	
	//Compute the binary image
	for( i = 0; i< ROWS; i++){
		for( j = 0; j< COLS; j++){
			if ( image[i][j] < hough_threshold){
				image[i][j] = 0;
			}
			else{
				image[i][j] = 255;
			}
		}
	}
	
	/* Write the binary image to a new image */
	strcpy( filename, "image");
	if (!( fp = fopen( strcat( filename, "-binary.ras" ), "wb" ) ))
	{
	  fprintf( stderr, "error: could not open %s\n", filename );
	  exit( 1 );
	}
	fwrite( head, 4, 8, fp );
	for ( i = 0 ; i < ROWS ; i++ ) fwrite( image[i], 1, COLS, fp );
	fclose( fp );
	
	//calculate rho and theta
	for( i = 0; i < ROWS; i++){
		for( j = 0; j < COLS; j++ )
		{
			if(image[i][j] == 255)
			{
				for( theta = 0; theta < 180; theta ++)
				{
					radian = pi/180 * theta;
					rho = j * cos(radian) - i * sin(radian);
					voting[(int)theta ][(int)rho +800] += 1; 
				}
			}
		}
	}
	
	max1 = 0;
	max2 = 0;
	max3 = 0;
	theta1 = 0; 
	theta2 = 0;
	theta3 = 0;
	rho1 = 0;
	rho2 = 0;
	rho3 = 0;
	
	//calculating max1
	for(i = 0; i<180; i++){
		for(j = 0; j< 1600; j++){
			if(max1<voting[i][j])
			{
				max1 = voting[i][j];
				theta1 = i;
				rho1 = j;
			}
		}
	}
	//remove max1 from values
	voting[theta1][rho1] = 0;

	//calculating max2
	for(i = 0; i<180; i++){
		for(j = 0; j< 1600; j++){
			if(max2<voting[i][j])
			{
				max2 = voting[i][j];
				theta2 = i;
				rho2 = j;
			}
		}
	}
	//remove max2 from values
	voting[theta2][rho2] = 0;
	
	//calculate max3
	for(i = 0; i<180; i++){
		for(j = 0; j< 1600; j++){
			if(max3<voting[i][j])
			{
				max3 = voting[i][j];
				theta3 = i;
				rho3 = j;
			}
		}
	}
	
	rho1 -= 800;
	rho2 -= 800;
	rho3 -= 800;
	
	printf("line 1 : rho = %d, theta = %d, votes = %d\n", rho1, theta1, max1);
	printf("line 2 : rho = %d, theta = %d, votes = %d\n", rho2, theta2, max2);
	printf("line 3 : rho = %d, theta = %d, votes = %d\n", rho3, theta3, max3);
	
	radian1 = pi/180 * theta1;
	radian2 = pi/180 * theta2;
	radian3 = pi/180 * theta3;
	
	//clearing old image
	clear(image);
	
	//Creating voting image
	for(i = 0; i< ROWS; i++){		
		j = (float) i*tan(radian1) + rho1/cos(radian1);
		if(j < COLS && j >= 0)
			image[i][j] = 255;
		j = (float) i*tan(radian2) + rho2/cos(radian2);
		if(j < COLS && j >= 0)
			image[i][j] = 255;
		j = (float) i*tan(radian3) + rho3/cos(radian3);
		if(j < COLS && j >= 0)
			image[i][j] = 255;
	}
		
	//Write the voting image
	strcpy(filename, "image");
	if(!(fp = fopen( strcat( filename, "-voting.ras"), "wb") ))
	{
		fprintf( stderr, "error: could not open %s\n", filename );
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for( i = 0; i< ROWS; i++) fwrite(image[i], 1, COLS, fp);
	fclose(fp);
			
	return 0;
}

void clear( unsigned char image[][COLS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLS ; j++ ) image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */
	
	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch ++; 
	head[6] = *ch;
	ch ++;
	head[5] = *ch;
	ch ++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch ++; 
	head[10] = *ch;
	ch ++;
	head[9] = *ch;
	ch ++;
	head[8] = *ch;
	
	ch = (char*)&num;
	head[19] = *ch;
	ch ++; 
	head[18] = *ch;
	ch ++;
	head[17] = *ch;
	ch ++;
	head[16] = *ch;
	

	/* Big-endian for unix */
	/*
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;
*/
}

