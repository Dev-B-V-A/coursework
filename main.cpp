#include "stdio.h"
#include "stdlib.h"

#define DEBUG 1

const int variables_count = 3;
const int functions_count = 4;

int read_file (const char *filename, int *values, int count);
void print_values (const int *values, int count, int columns = 4);
void make_polynom (int *values, int *polynoms, int count);


int main (int argc, char **argv)
{
    const char *filename = "input.txt";
    int function_values_count = (1 << variables_count);
    int values_count = function_values_count * functions_count;
    int values [values_count];

    read_file (filename, values, values_count);

    print_values(values, values_count);

    int polynom[values_count];

    make_polynom(values, polynom, function_values_count);

    print_values(polynom, values_count, function_values_count);
    return 0;
}

int read_file (const char *filename, int *values, int count)
{
    FILE *fp = nullptr;
    if (!(fp = fopen (filename, "r")))
    {
        printf ("Cannot open file %s\n", filename);
        return -1;
    }
    int index = 0;
    while (!feof (fp) && index < count)
    {
        if (fscanf (fp, "%d", values + index) != 1)
        {
            printf ("Cannot read element with number %d\n", index);
            fclose (fp);
            return -2;
        }
        ++index;
    }
    fclose (fp);
    return 0;
}

void print_values (const int *values, int count, int columns)
{
  for (int i = 1; i <= count; ++i)
  {
      printf ("%d | ", values[i - 1]);
      if (i % columns == 0)
          printf ("\n");
  }
}

void make_polynom(int *values, int *polynoms, int count)
{
    for (int fun_num = 0; fun_num < functions_count; fun_num++)
    {
        for (int i = 0; i < count; i++)
        {
            polynoms[fun_num * count + i] = values[fun_num];
            for (int j = 0; j < count - i -1; j++)
                values[fun_num + functions_count * j] = (values[fun_num + functions_count * j] + values[fun_num + functions_count * (j + 1)]) % 2;
        }
    }
}
