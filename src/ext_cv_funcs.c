/*
 * ext_cv_funcs.c
 *
 *  Created on: 24/07/2011
 *      Author: kintaro
 */

int CCAnalysis(int Connectivity) {

	/* Finds connected components, like having a BW image,
	 * Background = White, Foreground = Black;
	 */
	/* definition of a structure for describing runs of pixels in a row */
	struct run {
	   int  xl;     /* starting position (column number)  */
	   int  xr;     /* ending position (x1 <= x2)         */
	   int  y;      /* row number                         */
	   long acum_x;
	   long acum_y;
	   int  n_pix;
	   int  id;     /* the id (index) of the containing component  */
	};

	/* definition of a structure to describe runs of pixels in a row,
	 * without id number
	 */
	struct CompRun
	{
		int xl;
		int xr;
		int y;
	};

	/* definition of a structure to describe a component */
	struct Components {
		int 	 in_use;
		int 	 NumOfRuns;
		CompRun *RunsArray;
		long 	 acum_x;
		long 	 acum_y;
		int 	 n_pix;
	};

	const int COMPNT_ALLOC_STEP = 50;

	/* 0 for 4 - neighbourhood connectivity */
	/* 1 for 8 - neighbourhood connectivity */
	int ConnectivityConstant = 8;

	/* general definitions */
	int i, j, k, l;

	/* Definitions for the normal analysis */
	int in_black;
	int above_runs_num, left_runs_num;   /* counters for above and left runs */
	int overlaps;    /* number of runs overlapping from above with 'this_run' */
	int new_compnt_label = 0;
	int allocation_size;
	int n_active_components;
	int n_runs;
	Components *component;
	run *above_run, *left_run, this_run;

	/* allocate memory for runs and components */
	above_run = (run *) malloc ((m_Width/2+1)*sizeof(run));
	//above_run = new run[m_Width/2 + 1];
	left_run  = (run *) malloc ((m_Width/2+1)*sizeof(run));
	//left_run = new run[m_Width/2 + 1];
	component = (Components *) malloc ((COMPNT_ALLOC_STEP)*sizeof(Components));
	//component = new Components[COMPNT_ALLOC_STEP];
	allocation_size = COMPNT_ALLOC_STEP;

	above_runs_num = 0;
	n_active_components = 0;
	for (i = 0; i < m_Height; i++) {
		in_black = 0;
		left_runs_num = 0;  /* no runs to the left at this point */

		for (j = 0; j < m_Width; j++) {

			 /* Normal Analysis, PrivateGetByte(x, y) returns
			  * the colour at position (x,y)
			  */
			if ( PrivateGetByte(j, i) == BILEVEL_BLACK ) {
				if (!in_black) {
					// we found the beginning of the run
					this_run.xl = j;
					this_run.y  = i;
					in_black = 1;
				}
				
				this_run.acum_x += j;
				this_run.acum_y += i;
				this_run.n_pix++;
			}

			if ( (in_black && PrivateGetByte(j, i) == BILEVEL_WHITE ) ||
				 (j == (m_Width - 1) && in_black))				{
				// we found the end of the run
				in_black = 0;
				
				// assign x2 value to the run
				if (j == m_Width - 1) {
					if ( PrivateGetByte(j, i) == BILEVEL_WHITE ) {
						this_run.xr = j-1;
					} else {
						//Last pixel is foreground, include it in run
						this_run.xr = j;
				    }
				} else {
					this_run.xr = j - 1;
				}

				// check for overlaps with runs in the row above -> 4 or 8 <- NEIGHBOURS
				overlaps = 0;  // no overlaps initially
				for (k = 0; k < above_runs_num; ++k) {
				
					if ((above_run[k].xl - ConnectivityConstant <= this_run.xr) &&
						(this_run.xl <= above_run[k].xr + ConnectivityConstant)) {
						
					 // overlapping
					 // assign to the run the label (id) of the run above if it is the first overlap found
						if (overlaps == 0) {
							this_run.id = above_run[k].id;
							
							// Add this run to component
							n_runs = component[this_run.id].NumOfRuns+1;
							CompRun *temp = (CompRun *) malloc (n_runs * sizeof (CompRun));
							memcpy(temp, component[this_run.id].RunsArray, component[this_run.id].NumOfRuns * sizeof(CompRun));
							
							free(component[this_run.id].RunsArray); /*!!! Posible fallo*/
							
							component[this_run.id].RunsArray = temp;
							component[this_run.id].RunsArray[component[this_run.id].NumOfRuns].xl=this_run.xl;
							component[this_run.id].RunsArray[component[this_run.id].NumOfRuns].xr=this_run.xr;
							component[this_run.id].RunsArray[component[this_run.id].NumOfRuns].y=this_run.y;
							component[this_run.id].NumOfRuns++;
							component[this_run.id].acum_x += this_run.acum_x;
							component[this_run.id].acum_y += this_run.acum_y;
							component[this_run.id].n_pix  += this_run.n_pix;
						} else {  // not the first overlapping
							if (this_run.id != above_run[k].id) {
							   // the runs belong to different components
								//Copy all the Runs of the component that owns the above run to current component//
								n_runs = component[this_run.id].NumOfRuns+component[above_run[k].id].NumOfRuns;
								CompRun *temp = (CompRun *) malloc (n_runs * sizeof (CompRun));
								memcpy(temp, component[this_run.id].RunsArray, component[this_run.id].NumOfRuns * sizeof(CompRun));
								memcpy(&(temp[component[this_run.id].NumOfRuns]), component[above_run[k].id].RunsArray, component[above_run[k].id].NumOfRuns * sizeof(CompRun));
								free(component[this_run.id].RunsArray); /*!!! Posible fallo*/
								component[this_run.id].RunsArray = temp;
								component[this_run.id].NumOfRuns+=component[above_run[k].id].NumOfRuns;
								component[this_run.id].acum_x += component[above_run[k].id].acum_x;
								component[this_run.id].acum_x += component[above_run[k].id].acum_y;
								component[this_run.id].n_pix  += component[above_run[k].id].n_pix;
								
								component[above_run[k].id].in_use=0;
								n_active_components--;
								
								free(component[above_run[k].id].RunsArray); /*!!! Posible fallo*/
								
								//change the ids of all the above component's runs to the current components id//
								int aboveID=above_run[k].id;
								
								for (l=0; l<above_runs_num; l++)
									if (above_run[l].id==aboveID) above_run[l].id=this_run.id;
								for (l=0; l<left_runs_num;l++)
									if (left_run[l].id==aboveID) left_run[l].id=this_run.id;
							}
						}
						
						overlaps++;
					}
				}
				if (overlaps == 0) { // no overlaps with runs above
					// assign a new label to the run and begin a new component
					this_run.id = new_compnt_label;
					component[new_compnt_label].in_use = TRUE;
					component[new_compnt_label].NumOfRuns=1;
					component[new_compnt_label].RunsArray=new CompRun[1];
					component[new_compnt_label].RunsArray[0].xl=this_run.xl;
					component[new_compnt_label].RunsArray[0].xr=this_run.xr;
					component[new_compnt_label].RunsArray[0].y=this_run.y;
					
					component[new_compnt_label].acum_x = this_run.acum_x;
					component[new_compnt_label].acum_x = this_run.acum_y;
					component[new_compnt_label].n_pix  = this_run.n_pix;
					
					new_compnt_label++;
					n_active_components++;

					// check if more memory is needed
					if (new_compnt_label == allocation_size) {
						Components * temp = new Components[allocation_size + COMPNT_ALLOC_STEP];
						memcpy(temp, component, allocation_size * sizeof(Components));
						delete component;
						component = temp;
						allocation_size += COMPNT_ALLOC_STEP;
					}
				}
				
				// put 'this_run' into the 'left_run' array
				left_run[left_runs_num].id = this_run.id;
				left_run[left_runs_num].xl = this_run.xl;
				left_run[left_runs_num].xr = this_run.xr;
				left_run[left_runs_num].y = this_run.y;
				left_runs_num++;
			}
		}

		// put all runs in 'left_run' (all runs in current row) into 'above_run'
		for (k = 0; k < left_runs_num; ++k) {
			above_run[k].id = left_run[k].id;
			above_run[k].xl = left_run[k].xl;
			above_run[k].xr = left_run[k].xr;
			above_run[k].y = left_run[k].y;
		}
		
		above_runs_num = left_runs_num;
	}

	delete left_run;
	delete above_run;

	//Do something with the new components (e.g. calculate the center of each component)
	for (k=0;k<new_compnt_label;k++) {
		if (component[k].in_use==TRUE) {
			//DO SOMETHING HERE
		}
	}
	
	free(component); /*!!! Posible fallo*/

	return TRUE;
}
