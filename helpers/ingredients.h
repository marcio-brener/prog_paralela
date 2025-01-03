int count_ingredient_in_lines(FILE *file, const char *ingredient, int start_line, int end_line);
int find_or_add(IngredientCount ***ingredients, int *ingredients_size, const char *ingredient);
void remove_quotes_and_brackets(char *str);
void extract_ingredients(char *line, IngredientCount ***ingredients, int *ingredients_size);
void sort_ingredients(IngredientCount **ingredients, int size);
void print_biggest_three_ingredients(IngredientCount **ingredients);