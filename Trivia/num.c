#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int main( int argc, char **args )
{
	FILE * fpr;
	FILE * fpw;
	int count;
	char sz[1000];

	if (argc < 2)
	{
		fprintf(stderr,"Usage:\nNUM infile [outfile] [start#]\n\n");
		return 0;
	}

	if ((fpr = fopen((char *)(args[1]),"r")))
	{
		if ((fpw = fopen((argc >= 3 ? (char *)(args[2]) : "out.txt"),"w")))
		{
			count = 1;
			if (argc >= 4)
			{
				sscanf(args[3], "%i", &count);
			}

			while (fgets(sz, sizeof(sz), fpr))
			{
				if ((sz[0] == '#' && sz[1] == '#') ||
					(sz[0] == '@' && sz[1] == '@'))
				{
					fprintf(fpw,"%c%c%d\n",sz[0],sz[1],count++);
				}
				else
				{
					fprintf(fpw,"%s",sz);
				}
			}
			fclose(fpw);
		}
		fclose(fpr);
	}
	return 0;
}