#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define WIDTH 600
#define HEIGHT 600
#define MAX_ITERATIONS 256

int main(int argc, char** argv) {
    double x0;
    double y0;
    double x;
    double y;
    double x_temp;
    int i;
    int j;
    int k;
    int n;
    int myrank, mysize;
    int color[WIDTH][HEIGHT];
    double scale_real = 3.0 / WIDTH;
    double scale_imag = 3.0 / HEIGHT;

    double start, end, comm_start, comm_end, comm_time;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    MPI_Comm_size(MPI_COMM_WORLD, &mysize);

    int lheight = HEIGHT / mysize;
    
    int srow = myrank * lheight;
    
    int erow = (myrank + 1) * lheight;
    
    if (myrank == mysize - 1) {
        erow = HEIGHT;
    }


    int local_color[lheight][WIDTH];

    
    start = MPI_Wtime();


    for (i = srow; i < erow; i++) {
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
                local_color[i-srow][j] = 0;
            } else {
                local_color[i-srow][j] = k;
            }
        }
    }
    

    
    

    comm_start = MPI_Wtime();

    MPI_Gather(local_color, lheight * WIDTH, MPI_INT, color, lheight * WIDTH, MPI_INT, 0, MPI_COMM_WORLD);
    
    comm_end = MPI_Wtime();
    
    comm_time = comm_end - comm_start;

    end = MPI_Wtime();

    if (myrank == 0) {
        FILE* fp = fopen("mandelbrot2.ppm", "wb");
        fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
        for (i = 0; i < WIDTH; i++) {
            for (j = 0; j < HEIGHT; j++) {
                n = color[i][j];
                fputc((n % 256), fp);
                fputc((n / 256 % 256), fp);
                fputc((n / 65536 % 256), fp);
            }
        }
        fclose(fp);
        
        printf("Communication time taken: %f seconds\n", comm_time);
        
        printf("Total time taken: %f seconds\n", (end-start));
    }

    MPI_Finalize();

    return 0;
}
