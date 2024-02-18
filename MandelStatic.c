#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define WIDTH 600
#define HEIGHT 600
#define MAX_ITERATIONS 256

int main(int argc, char** argv) {
 int i;
    int j;
    int k;
    int l;
    double x0;
    double y0;
    double x;
    double y;
    double x_temp;
   
    int myrank, worldsize;
    int pixel[WIDTH][HEIGHT];
    double scale_real = 3.0 / WIDTH;
    double scale_imag = 3.0 / HEIGHT;

    

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldsize);
    
    
    double start, end;
    start = MPI_Wtime();
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            x0 = i * scale_real - 1.5;
            y0 = j * scale_imag - 1.5;
            x = 0.0;
            y = 0.0;
            k = 0;
            while (k < MAX_ITERATIONS && (x*x + y*y) < 4.0) {
                x_temp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = x_temp;
                k++;
            }
            if (k == MAX_ITERATIONS) {
                pixel[i][j] = 0;
            } else {
                pixel[i][j] = k;
            }
        }
    }
    end = MPI_Wtime();

    if (myrank == 0) {
        FILE* fp = fopen("mandelbrot1.ppm", "wb");
        fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
        for (i = 0; i < WIDTH; i++) {
            for (j = 0; j < HEIGHT; j++) {
                l = pixel[i][j];
                fputc((l % 256), fp);
                fputc((l / 256 % 256), fp);
                fputc((l / 65536 % 256), fp);
            }
        }
        fclose(fp);
        printf("Time taken: %f seconds\n", end - start);
    }
    MPI_Finalize();
   		return 0;
}
