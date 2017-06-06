/*** Allard Group jun.allard@uci.edu                    ***/

void initializeSummary();
void finalizeSummary();
void dataRecording();

/*******************************************************************************/
//  GLOBAL VARIABLES for output control
/*******************************************************************************/

double reeBar_sum, ree2Bar_sum, rMBar_sum;
long POcclude_sum[NMAX], Prvec0_sum[NMAX], POccludeBase_sum, PDeliver_sum[NMAX], PMembraneOcclude_sum[NMAX];
double reeBar, ree2Bar, POcclude[NMAX], POccludeBase, PDeliver[NMAX], Prvec0[NMAX],PMembraneOcclude[NMAX], reeiSite[NMAX], rMBar;
double occupied[NMAX];
double binSize;

/********************************************************************************************************/
void initializeSummary()
{
    // summary variables
    reeBar_sum   = 0;
    ree2Bar_sum  = 0;
    for(iy=0;iy<iSiteTotal;iy++)
    {
        POcclude_sum[iy]          = 0;
        Prvec0_sum[iy]            = 0;
        PMembraneOcclude_sum[iy]  = 0;
    }
    POccludeBase_sum = 0;
//    for(ib=0; ib<bSiteTotal; ib++)
//    {
//        PDeliver[ib]=0;
//    }
    rMBar_sum    = 0;
    
    binSize = (double)(2*N) / NBINSPOLYMER;

}

/********************************************************************************************************/
void finalizeSummary()
{
    // finalize summary statistics
    reeBar = reeBar_sum/(double)(nt-NTCHECK);
    ree2Bar = ree2Bar_sum/(double)(nt-NTCHECK);

	
    for(iy=0;iy<iSiteTotal;iy++)
    {
        POcclude[iy] = (double)POcclude_sum[iy]/(double)(nt-NTCHECK);
        Prvec0[iy] = (double)Prvec0_sum[iy]/(4/3*PI*pow((double)N/(double)NBINS,3))/(double)(nt-NTCHECK);
        PMembraneOcclude[iy] = (double)PMembraneOcclude_sum[iy]/(double)(nt-NTCHECK);
    }
    
    POccludeBase = (double)POccludeBase_sum/(double)(nt-NTCHECK);
    
//    for(ib=0;ib<bSiteTotal;ib++)
//    {
//        PDeliver[ib] = (double)PDeliver_sum[ib]/(double)(nt-NTCHECK);
//    }
    
    rMBar = rMBar_sum/(double)(nt-NTCHECK);
    
    if (!verboseTF)
    {
        fList = fopen(listName, "a");
        
        
        if (CD3ZETA)
        {
            sscanf(occupiedSites,"%lf_%lf_%lf_%lf_%lf_%lf", &occupied[0],&occupied[1],&occupied[2],&occupied[3], &occupied[4],&occupied[5]);
            
            for (iy=0; iy<iSiteTotal;iy++)
            {
                fprintf(fList, "%lf ", occupied[iy]);
            }
            
            fprintf(fList, "%s ", occupiedSitesNoSpace);
        }
        
        fprintf(fList, "%ld %f %f %f %ld %f %f %f %e",
                N,           // 1
                irLigand,    // 2
                brLigand,    // 3
                Force,       // 4
                nt,          // 5
                ksStatistic, // 6
                reeBar,      // 7
                ree2Bar,     // 8
                rMBar);      // 9
        
        for (iy=0;iy<iSiteTotal;iy++)
        {
            fprintf(fList, " %ld %e %e %e %e",
                iSite[iy], //10 + 4*iBind
                POcclude[iy], //11 + 4*iBind
                1-POcclude[iy], //12 + 4*iBind
                PMembraneOcclude[iy], //13 +4*iBind
                Prvec0[iy]); //14 + 4*iBind
        
        }

        
        fprintf(fList, " %d %e %e", -1, POccludeBase, 1-POccludeBase);
        
        for (ib=0;ib<bSiteTotal;ib++)
        {
            fprintf(fList, " %ld",
                bSite[ib]);
                    //PDeliver[ib]);
        }
        
        if(0)
        {
            for (iy=0;iy<iSiteTotal;iy++)
            {
                iSiteCurrent=iSite[iy];
                for (j=0;j<NBINSPOLYMER;j++)
                {
                    fprintf(fList, " %ld", polymerLocationCounts[iSiteCurrent][j]);
                }
            }
        }
        
        
        if(0)
        {
            iSiteCurrent=iSite[iy];
            for (j=0;j<NBINSPOLYMER;j++)
            {
                fprintf(fList, " %ld", polymerLocationCounts[N-1][j]);
            }
        }
        
        
        fprintf(fList, "\n");
        fclose(fList);
    }
}

/********************************************************************************************************/
// Prepare stuff and optionally write to file - this function is called each timestep
void dataRecording()
{
	
    // end-to-end distance
	ree      = sqrt(  r[N-1][0]*r[N-1][0]       + r[N-1][1]*r[N-1][1]     + r[N-1][2]*r[N-1][2]);
	
    // distance from base to iSite
    for(iy=0;iy<iSiteTotal;iy++)
    {
        iSiteCurrent = iSite[iy];
        reeiSite[iy] = sqrt(r[iSiteCurrent][0]*r[iSiteCurrent][0] + r[iSiteCurrent][1]*r[iSiteCurrent][1] + r[iSiteCurrent][2]*r[iSiteCurrent][2]);
	}

    // distance of tip to membrane
	rM = r[N-1][2];
	
    // height (max distance to membrane)
	if  (0)
    {
        rH = 0;
        for(i=0;i<N;i++)
            if (r[i][2]>rH)
                rH = r[i][2];
    }
	
    // Verbose output: One line is written each iteration.
    if (verboseTF)
    {
        
        if ( (nt > NTCHECK && nt <= NTCHECK+4000) ) //only output 4000 runs, after initial transient
        {
        // output results to file
        fList = fopen(listName, "a");
        fprintf(fList, "%ld %f %f %f %f %f %f %f %f %f %ld",
                nt,               // 1
                ree,              // 2
                rM,               // 3
                rH,               // 4
                E,                // 5
                dChi[0],          // 6
                dChi[1],          // 7
                rate[0],          // 8
                rate[1],          // 9
                ksStatistic,      // 10
        		constraintProposalsTotal);// 11
            
             if (VISUALIZE)
             {
                for (i=0;i<N;i++)
                {
                    fprintf(fList, " %f %f %f", r[i][0],r[i][1],r[i][2]);
                }
                 
                for (i=0;i<iSiteTotal;i++)
                {
                    fprintf(fList, " %f %f %f", iLigandCenter[i][0], iLigandCenter[i][1], iLigandCenter[i][2]);
                }
                if(baseBoundYN)
                {
                    for (i=0;i<3;i++)
                    {
                        fprintf(fList," %f", baseCenter[i]);
                    }
                }
                 
             }
        
            for(iy=0;iy<iSiteTotal;iy++)
            {
                fprintf(fList, " %ld",stericOcclusion[iy]);
            }
            
            for(iy=0;iy<iSiteTotal;iy++)
            {
                fprintf(fList, " %ld",membraneOcclusion[iy]);
            }
        
            fprintf(fList, " %ld", stericOcclusionBase);

            
        
        
//        for(ib=0;ib<bSiteTotal;ib++)
//        {
//            fprintf(fList, " %ld", boundToBaseDeliver[ib]);
//        }
        
            fprintf(fList, "\n");

            fclose(fList);
        }
    } // finished verbose output
    
    // Note:  eliminates initial transient
    if (nt>NTCHECK)
    {
        // Summary statistics (these will be written at the end of the run)
        reeBar_sum   += ree;
        ree2Bar_sum  += ree*ree;
        
        for(iy=0;iy<iSiteTotal;iy++)
        {
			POcclude_sum[iy] += (long)(stericOcclusion[iy]>0);
			Prvec0_sum[iy]   += (long)(reeiSite[iy] < (double)N/(double)NBINS);
            PMembraneOcclude_sum[iy] += (long)(membraneOcclusion[iy]>0);
			
        }
        POccludeBase_sum += (long)(stericOcclusionBase>0);
        //PDeliver_sum[ib] += (long)(boundToBaseDeliver>0);
        rMBar_sum    += rM;

        // update bins for KS test (fabs(rM)+ree will never be larger than 2N, so use 2N to normalize)
		convergenceVariableCounts[(long)floor(NBINS*(fabs(rM)+ree)/(2*N))]++;
        
        // Distributions for polymer location

        for(i=0;i<N;i++)
        {
            polymerLocationCounts[i][(long)floor(((double)r[i][2]+N)/binSize)]++;
        }
                                     
    }
	return;
	
}

