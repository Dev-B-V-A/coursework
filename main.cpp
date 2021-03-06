#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define DEBUG 1

const int variables_count = 4;
const int functions_count = 4;
const int function_prod_count = 11;

const int mask_x1 = 8;
const int mask_x2 = 4;
const int mask_x3 = 2;
const int mask_x4 = 1;

int read_file(const char *filename, int *values, int count);

void print_values(const int *values, int count, int columns = 4,
                  bool print_polynom = true);

void make_polynom(int *values, int *polynoms, int count);

void product_polynoms(int *p1, int *p2, int *res, int *table, int count);

void product_all_polynoms(int *polynoms, int *res, int count);

int check_polynoms(int *polynoms, int *products, int count);

double compute_nonlinear(int *func_values, int count);

int main(int argc, char **argv) {
  const char *filename = "input.txt";
  int function_values_count = (1 << variables_count);
  int all_input_values_count = function_values_count * functions_count;

  int values[all_input_values_count];
  memset(values, all_input_values_count, 0);

  read_file(filename, values, all_input_values_count);

  printf("Functions input values:\n");
  print_values(values, all_input_values_count);

  int functions_polynoms[all_input_values_count];
  int copy_values[all_input_values_count];
  memcpy(copy_values, values, all_input_values_count * sizeof(int));

  make_polynom(copy_values, functions_polynoms, function_values_count);

  printf("Functions polynoms:\n");
  print_values(functions_polynoms, all_input_values_count,
               function_values_count);

  int all_products_polynoms[function_prod_count * function_values_count];
  for (int i = 0; i < function_prod_count * function_values_count; i++)
    all_products_polynoms[i] = 0;

  product_all_polynoms(functions_polynoms, all_products_polynoms,
                       function_values_count);

#if DEBUG
  printf("All Products:\n");
  print_values(all_products_polynoms,
               function_values_count * function_prod_count,
               function_values_count);
#endif

  printf("Check polynoms\n");
  check_polynoms(functions_polynoms, all_products_polynoms,
                 function_values_count);

  printf("NonLinear:\n");
  for (int f_num = 0; f_num < functions_count; f_num++) {
    double nonlinear = compute_nonlinear(values + f_num * function_values_count,
                                         function_values_count);
    printf("Function %d nonlinear = %f\n", f_num, nonlinear);
  }

  return 0;
}

int read_file(const char *filename, int *values, int count) {
  FILE *fp = nullptr;
  if (!(fp = fopen(filename, "r"))) {
    printf("Cannot open file %s\n", filename);
    return -1;
  }
  int index = 0;
  while (!feof(fp) && index < count) {
    if (fscanf(fp, "%d", values + index) != 1) {
      printf("Cannot read element with number %d\n", index);
      fclose(fp);
      return -2;
    }
    ++index;
  }
  fclose(fp);
  return 0;
}

void print_values(const int *values, int count, int columns,
                  bool print_polynom) {

  for (int i = 1; i <= count; ++i) {
    printf("%d | ", values[i - 1]);
    if (i % columns == 0)
      printf("\n");
  }
  if (print_polynom) {
    for (int i = 1; i <= count; i++) {
      int index = i - 1;
      if (values[index] == 1) {
        if (index == 0)
          printf("1");
        for (int var = 0; var < variables_count; var++) {
          if (index & (1 << var))
            printf("x%d", variables_count - var);
        }
        printf(" + ");
      }
      if (i % columns == 0)
        printf("\n");
      
    }
  }
}

void make_polynom(int *values, int *polynoms, int count) {
  for (int fun_num = 0; fun_num < functions_count; fun_num++) {
    for (int i = 0; i < count; i++) {
      polynoms[fun_num * count + i] = values[fun_num];
      for (int j = 0; j < count - i - 1; j++)
        values[fun_num + functions_count * j] =
            (values[fun_num + functions_count * j] +
             values[fun_num + functions_count * (j + 1)]) %
            2;
    }
  }
}

void product_polynoms(int *p1, int *p2, int *res, int count) {
  for (int i = 0; i < count; i++) {
    if (p1[i] == 1) {
      for (int j = 0; j < count; j++)
        if (p2[j] == 1) {
          int value = i | j;
          res[value] = res[value] == 0 ? 1 : 0;
        }
    }
  }
}

void product_all_polynoms(int *polynoms, int *res, int count) {

  int all_mult = 0;

  for (int i = 0; i < functions_count; i++) {
    for (int j = i + 1; j < functions_count; j++) {
      product_polynoms(polynoms + i * count, polynoms + j * count,
                       res + count * all_mult, count);
      all_mult++;
    }
  }
  for (int i = 0; i < functions_count; i++) {
    int temp[count];
    for (int k = 0; k < count; k++)
      temp[k] = 0;
    int p_num1 = (i + 1) % functions_count;
    int p_num2 = (i + 2) % functions_count;
    int p_num3 = (i + 3) % functions_count;

    product_polynoms(polynoms + p_num1 * count, polynoms + p_num2 * count, temp,
                     count);
    product_polynoms(polynoms + p_num3 * count, temp, res + count * all_mult,
                     count);
    all_mult++;
  }
  int p1_p2_index = 0;
  int p3_p4_index = 5;
  product_polynoms(res + count * p1_p2_index, res + count * p3_p4_index,
                   res + count * all_mult, count);
}

int check_polynoms(int *polynoms, int *products, int count) {
  for (int num = 0; num < functions_count; num++)
    for (int index = 0; index < count; index++)
      if (polynoms[num * count + index] == 1 && (index & (1 << num))) {
        printf("function is not fine f[%d], index = %d\n", num, index);
        return -1;
      }
  int product_index = 0;
  for (int i = 0; i < functions_count; i++)
    for (int j = i + 1; j < functions_count; j++) {
      for (int index = 0; index < count; index++)
        if (products[product_index * count + index] == 1)
          if ((index & (1 << i)) && (index & (1 << j))) {
            printf("Function f_i * f_j is not fine f[%d,%d]\n", i, j);
            return -2;
          }
      product_index++;
    }

  for (int i = 0; i < functions_count; i++) {
    for (int index = 0; index < count; index++) {
      if (products[product_index * count + index] == 1) {
        int p_num1 = (i + 1) % functions_count;
        int p_num2 = (i + 2) % functions_count;
        int p_num3 = (i + 3) % functions_count;
        if ((index & (1 << p_num1)) && (index & (1 << p_num2)) &&
            (index & (1 << p_num3))) {
          printf("Function f_i * f_j * f_k is not fine f[%d,%d,%d]\n", p_num1,
                 p_num2, p_num3);
          return -3;
        }
      }
    }

    product_index++;
  }

  if (products[product_index * count + count - 1] == 1) {
    printf("Function f_i * f_j * f_k * f_m is not fine\n");
    return -3;
  }

  return 0;
}

int w(int *func_values, int value, int count);

int w(int *func_values, int value, int count) {
  int sum = 0;
  for (int u = 0; u < count; u++) {
    int product = u & value;
    int scalar_product = 0;
    for (int index = 0; index < variables_count; index++)
      scalar_product =
          (scalar_product + ((product & (1 << index)) >> index)) % 2;

    int exp_index = (scalar_product + func_values[u]) % 2;
    sum += exp_index == 0 ? 1 : -1;
  }
  return sum;
}

double compute_nonlinear(int *func_values, int count) {
  int max = 0;
  for (int index = 0; index < count; index++) {
    int value = abs(w(func_values, index, count));
    if (value > max)
      max = value;
  }
  return 8 - 0.5 * max;
}
