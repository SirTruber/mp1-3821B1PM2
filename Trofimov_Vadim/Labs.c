# include <stdio.h>
# include <stdlib.h>
# include <time.h>
#define uint  unsigned int
int perm = 0, comp = 0;

double* Randomize(double a[], int size) {
	int i;
	srand(time(NULL));
	for (i = 0; i < size; i++) {
		a[i] = (rand() % 2001+(rand()%2000) / 1000.0) - 1000.0 ; // ����� ��������� ������� �����
	}
	return a;
}

void createCounters(double* data, long int* counters, long int N) {
	unsigned char* bp = (unsigned char*)data;
	unsigned char* dataEnd = (unsigned char*)(data + N);
	unsigned short int i;

	memset(counters, 0, 256*sizeof(double)*sizeof(long int)); //��������� ������ ������

	while (bp != dataEnd) {
		for (i = 0; i < sizeof(double); i++) {
			counters[256 * i + *(bp++)]++; //������� ������� ����������� ���������� ���� � ���� �������
		}
	}
}

void radixPass(short int offset, long int N, double* source, double* dest, long int* count) {
	double* sp;
	long int s = 0, c, i, * cp = count;
	unsigned char* bp;
	for (i = 256; i > 0; --i, ++cp) {
		c = *cp; // ��������������� ������, ��� ������� ������� ���� ���� ����� ������ ������
		*cp = s; // ��� � ����� ������� ���������� ����� � ���� ������
		s += c;
	}
	bp = (unsigned char*)source + offset;
	sp = source;
	for (i = N; i > 0; --i, bp = bp + sizeof(double), ++sp) { //���� ���������� ���������
		cp = count + *bp; //����� ����� ����� (�������, �� ��� ���������)
		dest[*cp] = *sp; //�������� ���� ����� �� ���� ������� � ����
		(*cp)++; //��������� 1 � �������� �������� �� ��������������� �������
	}
}

void SignedRadixSort(short int offset, long int N, double* source, double* dest, long int* count) {
	double* sp;
	long int s = 0, c, i, * cp = count, NumNeg = 0;
	unsigned char* bp;
	for (i = 128; i < 256; i++) {
		NumNeg = NumNeg + count[i];
	}
	s = NumNeg;
	cp = count;
	for (i = 0; i < 128; ++i, ++cp) {
		c = *cp;
		*cp = s;
		s += c;
	}
	s = count[255] = 0; cp = count + 254;
	for (i = 254; i >= 128; i--, cp--) {
		*cp += s;
		s = *cp;
	}
	bp = (unsigned char*)source + offset;
	sp = source;
	for (i = N; i > 0; i--, bp += sizeof(double), sp++) {
		cp = count + *bp;
		if (*bp < 128)
		{
			dest[*cp] = *sp;
			(*cp)++;
		}
		else
		{
			(*cp)--;
			dest[*cp] = *sp;
		}
	}
}

void RadixSort(double* in, double* out, long int* counters, long int N) {
	long int* count;
	unsigned char i;
	createCounters(in, counters, N);

	for (i = 0; i < sizeof(double) - 1; i++) {
		count = counters + 256 * i;
		radixPass(i, N, in, out, count);
		for (long int j = 0; j < N; j++) {
			in[j] = out[j];
			perm++;
		}
	}

	count = counters + 256 * i;
	SignedRadixSort(i, N, in, out, count);
	for (long int j = 0; j < N; j++) {
		in[j] = out[j];
		perm++;
	}
}

void callradixsort(double* in, long int N) {
	perm = 0; comp = 0;
	double* out = (double*)malloc(N * sizeof(double));
	long int* counters = (long*)malloc(sizeof(double) * 256 * sizeof(long int));
	RadixSort(in, out, counters, N);
	free(out);
	free(counters);
}

void Selection(double a[],int size) {
	int i,j, min;
	double tmp;
	for (i = 0; i < size - 1; i++) {
		min = i;
		for (j = i + 1; j < size; j++) {
			comp++;
			if (a[j] < a[min]) {
				min = j;					//������� ����������� ������� � �������
			}
		}
		comp++;
		if (min != i) {						// ������ ����������� ������� � ������
			tmp = a[i];
			a[i] = a[min];
			a[min] = tmp;
			perm++;
		}
	}
}

void Shell(double a[], int size) {
	int s, i, j;
	double tmp;
	for (s = size / 2; s > 0; s /= 2) {  //�������� ��� ����� ����������
		for (i = s; i < size; i++) {
			comp++;
			for (j = i - s; j >= 0 && a[j] > a[j + s]; j -= s) { //���������� �������� � ������ ������
				tmp = a[j];
				a[j] = a[j + s];
				a[j + s] = tmp;
				perm++;
			}
		}
	}
}

void merge(double* a, int left, int right) {
	int mid = (right-left) / 2 + left;
	int i = left;
	int j = mid + 1;
	int step = 0;
	double *c = (double*)malloc(right * sizeof(double));
	if (left >= right) return; // ���� ������� ������� �����������, ������� �� ������������ 
	merge(a, left, mid); merge(a, mid + 1, right); // ���������� ��������� ������ �� ����������
	for (step; step < right - left + 1; step++) {
		comp += 3;
		if ((j > right) || ((i <= mid) && (a[i] < a[j]))) { // ��������� �������� ������� ���������� ���� �������
			c[step] = a[i++];
			perm++;
		}
		else {
			c[step] = a[j++];
			perm++;
		}
	}
	for (step = 0; step < right - left + 1; step++) {
		a[left + step] = c[step];      // ������������ ��������������� ��������� ������ � ��������
	}
}

void main() {
	int size, variant,i,step=1;
	double *a = NULL;
	printf("enter the size of the array\n");
	scanf_s(" %d", &size);
	a = (double*)malloc(size * sizeof(double));
	Randomize(a, size);
	printf("enter a step of passage\n");
	scanf_s("%d", &step); 
	do
	{
		system("cls");
		printf("Select the sort type\n");
		printf("1.Selection\n");
		printf("2.Shell\n");
		printf("3.Merge\n");
		printf("4.Radix\n");
		printf("5.Randomize array\n");
		printf("6.Change size\n");
		printf("0.Exit\n");
		scanf_s(" %d", &variant);
			switch (variant)
			{
			case 1:
				for (i = 2; i < size;i+=step) {
					Randomize(a, i);
					Selection(a, i);
					printf("%d %d ", i-1 ,comp + perm);
					comp = 0; perm = 0;

				}
				system("pause");
				break;
			case 2:
				for (i = 2; i < size; i += step) {
					Randomize(a, i);
					Shell(a, i);
					printf("%d %d ",i-1, comp + perm);
					comp = 0; perm = 0;
				}
				system("pause");
				break;
			case 3:
				for (i = 2; i < size; i += step) {
					Randomize(a, i);
					merge(a, 0, i - 1);
					printf("%d %d ",i-1, comp + perm);
					comp = 0; perm = 0;
				}
				system("pause");
				break;
			case 4:
				for (i = 2; i < size; i += step) {
					Randomize(a, i);
					callradixsort(a, i);
					printf("%d %d ",i, comp + perm);
					comp = 0, perm = 0;
				}
				system("pause");
				break;
			case 5:
				Randomize(a, size);
				break;
			case 6:
				free(a);
				printf("enter the size of the array\n");
				scanf_s(" %d", &size);
				a = (double*)malloc(size * sizeof(double));
				Randomize(a, size);
				printf("enter a step of passage\n");
				scanf_s("%d", &step);
				break;
			case 0:
				break;
			default:
				printf("incorrent choice\n"); // �������� �� ��������� �����
				system("pause");
				break;
			}
	} while (variant != 0);	
}