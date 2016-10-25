/**
 * Kostra programu pro 3. projekt IZP 2015/16
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/slsrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>  //strcmp
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct objt_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/

void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

	c->size = 0;
	c->capacity = cap;
	c->obj = malloc(cap * sizeof(struct obj_t));

	if (c->obj == NULL)
		c->capacity = 0;

	return;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
	c->size = 0;
	c->capacity = 0;
	free(c->obj);
	c->obj = NULL;
	return;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */ 
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
	if (c->size == c->capacity)
		c = resize_cluster(c, c->capacity + 1);

	/*Lets hope, that this will never happen */
	if (c->size > c->capacity)
		c = resize_cluster(c, c->size + 1);

	if (c == NULL) {
		return;
	}
	
	c->obj[c->size] = obj;
	c->size++;
	return;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

	/* size of free space */
	int free_space = c1->capacity - c1->size;
	int i = 0;

	/* the size of free space is too small, realloc is needed */
	if (free_space < c2->size)
		c1 = resize_cluster(c1, c1->size + c2->size);

	/* error happens */
	if (c1 == NULL) {
		clear_cluster(c1);
		return;
	}

	/* merging clusters */
	while(c1->size <= c1->capacity && i < c2->size) {
		append_cluster(c1, c2->obj[i]);
		if (c1 == NULL)
			return;
		i++;
	}
	sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
	
	int i = idx;

	/* destroing cluster on index idx */
	clear_cluster(&carr[i]);

	/* moving rest of array */
	while (i < narr-1) {
		carr[i].size = carr[i+1].size;
		carr[i].capacity = carr[i+1].capacity;
		carr[i].obj = carr[i+1].obj;
		i++;
	}

	return narr-1;			
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
		float x1 = o1->x, y1 = o1->y;
		float x2 = o2->x, y2 = o2->y;
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

/*
 Pocita vzdalenost dvou shluku. Vzdalenost je vypoctena na zaklade nejblizsiho
 souseda.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

		float s = obj_distance(&c1->obj[0], &c2->obj[0]);
		float saved_distance = 0;
    for (int i = 0; i < c1->size; i++) {
			for (int k = 0; k < c2->size; k++) {
				saved_distance = obj_distance(&c1->obj[i], &c2->obj[k]);
			if (s > saved_distance)
				s = saved_distance;
		}
	}
	return s;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky (podle nejblizsiho souseda). Nalezene shluky
 identifikuje jejich indexy v poli 'carr'. Funkce nalezene shluky (indexy do
 pole 'carr') uklada do pameti na adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
	assert(narr > 0);

	float s = cluster_distance(&carr[0], &carr[1]);
	*c1 = 0;
	*c2 = 1;
	float saved_distance = 0;
	for (int i = 0; i < narr; i++) {
		for (int k = i + 1; k < narr-1; k++){
			saved_distance = cluster_distance(&carr[i], &carr[k]);
			if (s > saved_distance) {
				s = saved_distance;
				*c1 = i;
				*c2 = k;
			}
		}
	}
	return;
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

	FILE *src;
	src = fopen(filename, "r");
	if (src == NULL) {
		*arr = NULL;
		return -1;
	}

	int c;
	char buf[20];
	unsigned i = 0;

	/* loading "count=" */
	for (i = 0; i < 6; i++) {
		c = fgetc(src);
		buf[i] = c;
		if (i == 5) {
			buf[6] = '\0';
			if (strcmp(buf, "count=")) {
				*arr = NULL;
				if (fclose(src) == EOF)
					return -1;
				return -3; // invalid source file
			}
		}
	}

	/* loading the value of count */	
	for (i = 0; (c = fgetc(src)) != '\n'; i++) {
		if (c < '0' || c > '9')
			return -3; // invalid source file
		buf[i] = c;
		if (i > 17) {
			*arr = NULL;
			if (fclose(src) == EOF)
				return -1;
			return -3;
		}
	}

	buf[i] = '\0';

	struct obj_t obj;
	unsigned count = atoi(buf), row = 0, idx = 0;
	float x, y;
	unsigned id;

	i = 0;

	*arr = malloc(count * sizeof(struct cluster_t));
	if (*arr == NULL) {
		if (fclose(src) == EOF)
			return -1;
		return -2;
	}

	while (c != EOF && row < count) {
		c = fgetc(src);
		if ((c < '0' || c > '9') && c != ' ' && c != '\n' && c != EOF) {
			for(unsigned i = 0; i < row && row > 0; i++)
				clear_cluster(&((*arr)[i]));

			free(*arr);
			*arr = NULL;
			if (fclose(src) == EOF)
				return -1;
			return -3; // invalid source file
			}

		buf[i] = c;

		/* loading id into object */
		if (c == ' ' && idx == 0) {
			buf[i] = '\0';
			id = atoi(buf);

			if (id < 1) {
				for(unsigned i = 0; i < row; i++)
					clear_cluster(&((*arr)[i]));

				free(*arr);
				*arr = NULL;
				if (fclose(src) == EOF)
					return -1;
				return -3; // invalid source file
			}

			obj.id = id;
			i = 0;
			idx++;
			c = fgetc(src);
			buf[i] = c;
			if (c < '0' || c > '9') {
				for(unsigned i = 0; i < row && row > 0; i++)
					clear_cluster(&((*arr)[i]));

				free(*arr);
				*arr = NULL;
				if (fclose(src) == EOF)
					return -1;
				return -3; // invalid source file
				}
		}

		/* loading x into object */
		if (c == ' ' && idx == 1) {
			buf[i] = '\0';
			x = atof(buf);

			if (x > 1000 || x < 0) {
				for(unsigned i = 0; i < row; i++)
					clear_cluster(&((*arr)[i]));

				free(*arr);
				*arr = NULL;
				if (fclose(src) == EOF)
					return -1;
				return -3;
			}

			obj.x = x;
			i = 0;
			idx++;
			c = fgetc(src);
			buf[i] = c;
			if ((c < '0' || c > '9') && c != ' ') {
				for(unsigned i = 0; i < row && row > 0; i++)
					clear_cluster(&((*arr)[i]));

				free(*arr);
				*arr = NULL;
				if (fclose(src) == EOF)
					return -1;
				return -3; // invalid source file
				}
		}
		
		if (c == '\n' && idx != 2) {
			for(unsigned i = 0; i < row; i++)
				clear_cluster(&((*arr)[i]));
			free(*arr);
			*arr = NULL;
			if (fclose(src) == EOF)
				return -1;
			return -3; // invalid source file
		}

		/* loading y into object */
		if ((c == '\n' || c == EOF) && idx == 2) {
			buf[i] = '\0';
			y = atof(buf);

			if (y > 1000 || y < 0) {
				for(unsigned i = 0; i < row; i++)
					clear_cluster(&((*arr)[i]));
				free(*arr);
				*arr = NULL;
				if (fclose(src) == EOF)
					return -1;
				return -3; // invalid source file
			}

			obj.y = y;
			i = 0;
			idx = 0;
		}

		/* loading object into cluster */
		if (( c == EOF ||c == '\n') && idx == 0) {

			init_cluster(&(*arr)[row], CLUSTER_CHUNK);

			/* memory allocation error */
			if (&(*arr)[row] == NULL) {
				for(unsigned i = 0; i < row; i++)
					clear_cluster(&((*arr)[i]));
				free(*arr);
				*arr = NULL;
				free(*arr);
				if (fclose(src) == EOF)
					return -1;
				return -2;
			}

			append_cluster(&((*arr)[row]), obj);

			/* memory allocation error */
			if (&(*arr)[row] == NULL) {
				for(unsigned i = 0; i < row-1; i++)
					clear_cluster(&((*arr)[i]));
				free(*arr);
				*arr = NULL;
				if (fclose(src) == EOF)
					return -1;
				return -2;
			}

			row++;
			i = 0;
			c = fgetc(src);
			buf[i] = c;
			if ((c < '0' || c > '9') && c != ' ' && c != '\n' && c != EOF) {
				for(unsigned i = 0; i < row && row > 0; i++)
					clear_cluster(&((*arr)[i]));

				free(*arr);
				*arr = NULL;
				if (fclose(src) == EOF)
					return -1;
				return -3; // invalid source file
				}
		}

		if (i < 18) // i should not be 19 or more, becouse on index 19 will be '\0'
			i++;      // and size of array is 20

		else {
			for(unsigned i = 0; i < row; i++)
				clear_cluster(&((*arr)[i]));

			free(*arr);
			*arr = NULL;
			if (fclose(src) == EOF)
				return -1;
			return -3; // invalid source file, i >= 19
		}
	} 

	if (fclose(src) == EOF) {
		free(*arr);
		*arr = NULL;
		return -1;
	}
	
	if (row != count)
		return -3;
	return row;	
}// 205 rows lomg function

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char **argv) {

	int n = 0;
	if (argc == 3)
			n = atoi(argv[2]);

	if (argc == 2) 
		n = 1;

	/* invalid arguments */	
	if ((argc < 2 && argc > 3) || n <= 0) {
		fprintf(stderr, "Invalid arguments\n");
		return -1;	
	}



	int c1 = -1, c2 = -1;
	struct cluster_t *arr = NULL;

	/* loading the clusters */
	int count = load_clusters(argv[1], &arr);

	/* Error happens */
	if (count == -1) {
		fprintf(stderr, "Cannot open/close file\n");
		return -1;
	}
	if (count == -2) {
		fprintf(stderr, "Cannot allocate memory\n");
		return -1;
	}
	if (count == -3) {
		fprintf(stderr, "Invalid source file\n");
		return -1;
	}
	if (n > count) {
		/* free allocated memory */
		for (int i = 0; i < count; i++)
			clear_cluster(&arr[i]);
		free(arr);
		fprintf(stderr, "Too few objects in source file\n");
		return -1;
	}

	/* sorting objects into clusters */
	for(; count > n; count--) {
		find_neighbours (arr, count, &c1, &c2);	
		merge_clusters(&arr[c1], &arr[c2]);
		remove_cluster(arr, count, c2);
		sort_cluster(&arr[c1]);
	}

	/* output */
	print_clusters(arr, count);

	/* free allocated memory */
	for (int i = 0; i < count; i++)
		clear_cluster(&arr[i]);
	free(arr);

	return 0;
}
