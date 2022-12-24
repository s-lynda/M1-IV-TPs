
import java.io.*;
import java.math.*;
import java.util.*;
import java.time.Duration;

public class Hanoi_Tours 
{
	 static void Hanoi(int n, char Dep,char Arr, char Inter)
	    {
	        if (n == 1)
	        {
	        	System.out.println("Disque " + n + " de "+ Dep + " a "+ Arr);
	        }
	        else
	        {
	        Hanoi(n - 1, Dep, Inter, Arr);
	        System.out.println("Disque " + n + " de "+ Dep + " a "+ Arr);
	        Hanoi(n - 1, Inter, Arr, Dep);
	        }
	    }
	    public static void main(String args[])
	    {
	    	int N;
	    	do {
	    		Scanner sc = new Scanner(System.in);
	    		System.out.println("\n-Veuillez saisir le nombre de disques:");
	    	    N = sc.nextInt();
	    	}while(N<=1);
	        
	        double start, end,time;
	        start = System.currentTimeMillis();
	        
	        Hanoi(N,'A','C','B');
	        end = System.currentTimeMillis();
	        time = (end - start) / 1000F;
	        
	        System.out.println("\n-Temps d'execution: "+ time +" secondes");
	    }
	
}
