#include "br.h"
#include <math.h>
#include <float.h>

// estrutura para gerador de ruído
typedef struct
{
    unsigned int seed;
    float f, f2;
} NoiseContext;

// gerador de float entre 0 e 1
float randomf(unsigned int *seed)
{
    *seed = (*seed * 1103515245 + 12345) & 0x7fffffff;
    return (*seed) / (float)0x7fffffff;
}

// função de ruído usando contexto
float noise(NoiseContext *ctx)
{
    return randomf(&ctx->seed) * ctx->f2 - ctx->f;
}

// matriz auxiliar de float usada para cálculo intermediário
float **spatial_subdivision(float **H, int size, NoiseContext *ctx, int *newSize)
{
    int nb = 2 * (size - 1) + 1;

    float **H2 = malloc(nb * sizeof(float *));
    for (int i = 0; i < nb; i++)
    {
        H2[i] = malloc(nb * sizeof(float));
    }

    for (int i = 0; i < nb; i++)
    {
        int I = i >> 1;
        for (int j = 0; j < nb; j++)
        {
            int J = j >> 1;

            // interpola altura média
            H2[i][j] = (
                H[I][J] +
                H[I + (i & 1)][J] +
                H[I][J + (j & 1)] +
                H[I + (i & 1)][J + (j & 1)]
            ) / 4.0f;

            // adiciona ruído se necessário
            if ((i & 1) || (j & 1))
            {
                H2[i][j] += noise(ctx);
            }
        }
    }

    *newSize = nb;
    return H2;
}

// libera matriz float
void free_float_matrix(float **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

// libera matriz uint8_t
void free_matrix(uint8_t **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

// função principal que retorna matriz uint8_t**
uint8_t **generate_heightmap(int nbIters, unsigned int seed, float p1, float p2, int *outSize)
{
    NoiseContext ctx;
    ctx.seed = seed;

    // matriz inicial 2x2
    float **H = malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        H[i] = malloc(2 * sizeof(float));
        for (int j = 0; j < 2; j++)
        {
            H[i][j] = randomf(&ctx.seed);
        }
    }

    int size = 2;
    ctx.f = p1;

    // subdivide e aplica ruído
    for (int n = 0; n < nbIters; n++)
    {
        ctx.f2 = 2 * ctx.f;

        int newSize;
        float **H2 = spatial_subdivision(H, size, &ctx, &newSize);

        free_float_matrix(H, size);
        H = H2;
        size = newSize;

        ctx.f /= powf(2.0f, p2);
    }

    // encontra min e max
    float min = FLT_MAX;
    float max = -FLT_MAX;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            float h = H[i][j];
            if (h < min) min = h;
            if (h > max) max = h;
        }
    }

    // cria matriz final uint8_t**
    uint8_t **result = malloc(size * sizeof(uint8_t *));
    for (int i = 0; i < size; i++)
    {
        result[i] = malloc(size * sizeof(uint8_t));
        for (int j = 0; j < size; j++)
        {
            // normaliza para 0–255
            float norm = (H[i][j] - min) / (max - min);
            int val = (int)(norm * 255.0f + 0.5f); // arredonda
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            result[i][j] = (uint8_t)val;
        }
    }

    free_float_matrix(H, size);
    *outSize = size;
    return result;
}

#include <MiniFB.h>
#define WIDTH 512
#define HEIGHT 512

init(terrain)
{
    srand((unsigned int) time(NULL));

    int size = WIDTH;
    uint8_t** heightmap = generate_heightmap(9, rand(), 0.5f, 1, &size);

    // create window
    struct mfb_window* window = mfb_open_ex("heightmap", WIDTH, HEIGHT, WF_RESIZABLE);
    if (!window)
    {
        fprintf(stderr, "failed to open window\n");
        return;
    }

    // allocate pixel buffer (1 pixel = 32 bits = ARGB)
    uint32_t* buffer = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

    // convert heightmap to pixel data
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float v = heightmap[y][x]; // v should be 0.0 to 1.0
            uint8_t gray = (uint8_t)(v * 255.0f);
            uint32_t color = (0xFF << 24) | (gray << 16) | (gray << 8) | gray; // ARGB
            buffer[y * WIDTH + x] = color;
        }
    }

    // show once and wait for exit
    while (mfb_wait_sync(window))
    {
        mfb_update_ex(window, buffer, WIDTH, HEIGHT);
    }

    // cleanup
    free_matrix(heightmap, HEIGHT);
    free(buffer);

}