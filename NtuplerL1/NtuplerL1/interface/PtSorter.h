#ifndef PtSorter_h_
#define PtSorter_h_


// Bubble Sort Function for Descending Order 
void PtSorter(float pt[],float eta[], float phi[], int n)
{
      int i, j, flag = 1;    // set flag to 1 to start first pass
      int temp;             // holding variable
      int numLength = n; 
      for(i = 1; (i <= numLength) && flag; i++)
     {
          flag = 0;
          for (j=0; j < (numLength -1); j++)
         {
               if (pt[j+1] > pt[j])      // ascending order simply changes to <
              { 
                    temp = pt[j];             // swap elements
                    pt[j] = pt[j+1];
                    pt[j+1] = temp;

                    temp = eta[j];             // swap elements
                    eta[j] = eta[j+1];
                    eta[j+1] = temp;

                    temp = phi[j];             // swap elements
                    phi[j] = phi[j+1];
                    phi[j+1] = temp;

                    flag = 1;               // indicates that a swap occurred.
               }
          }
     }
     return;   //arrays are passed to functions by address; nothing is returned
}

#endif 
