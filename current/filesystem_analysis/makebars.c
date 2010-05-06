
#include <stdio.h>

int main()
{

  int label,count,tmp,maxcols,currcol;
  
  label = 4096;
  count = 0;

  maxcols = 32;
  
  currcol = 0;

  /* the first time we find a zero, everything else goes in the last bar */

  while(!feof(stdin))
    {
      if(scanf("%d",&tmp)==1)
	{
	  if(tmp<=label)
	    count++;
	  else
	    {
	      /* the first time we find a zero, 
		 everything else goes in the last bar */
	      if((count == 0)||(currcol++ == maxcols))
		{
		  while(!feof(stdin))
		    {
		      if(scanf("%d",&tmp)==1)
			{
			  count++;
			}
		    }
		  printf("%d %d\n",label,count);
		}
	      else
		{
		  printf("%d %d\n",label,count);
		  count = 0;
		  label += 4096;
		}
	    }
	}
    }

  return 0;
}
