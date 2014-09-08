#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "claret.h"
#include "app.h"

struct thread_data{
	int thread_id;
	int n1, *nig_num, n3, atype_mat[20], *atype;
	double *cd,r, rd, rr, inr, phi[3], phir;
};

struct thread_data thread_data_array[1];

void *ndk_ForcePt(void *threadarg){

	int i, j, k, c, n1;
	int i0, i1, i2, i3, i4, i5;
	double d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12;
	double dphir;
	struct thread_data *my_data;

	my_data = (struct thread_data *) threadarg;
	n1 = my_data->n1;

	pthread_exit(NULL);

}

void Native_PosixThreads(JNIEnv* env, jobject obj){
	// Thread handle
	pthread_t threads[0];
	thread_data_array[0].n1 = n1;

	// Create a new thread
	int result = pthread_create(&threads[0],NULL,ndk_ForcePt,(void*) &thread_data_array[0]);

	if (result)
	{
		exit(0);
	}
	pthread_exit(NULL);
}

void ndk_Force(){
	int i, j, k, c;
	int i0, i1, i2, i3, i4, i5;
	double d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12;
	double dphir;


	for (i = 0; i < n1; i++) nig_num[i] = 0;

				for (i = 0; i < n3; i += 3) {
					i0 = atype_mat[atype[i / 3]];
					for (j = i + 3; j < n3; j += 3) {
						d0 = cd[i] - cd[j];
						d1 = cd[i + 1] - cd[j + 1];
						d2 = cd[i + 2] - cd[j + 2];

						rd = d0 * d0 + d1 * d1 + d2 * d2;
						r = sqrt(rd);
						inr = 1. / r;

						i1 = atype_mat[atype[j / 3]];
						d7 = phir;

						if (i0 < 2 && i1 < 2) {
								d3 = pb * pol[i0][i1]
										* exp((sigm[i0][i1] - r) * ipotro[i0][i1]);

								dphir = (d3 * ipotro[i0][i1] * inr
										- 6 * pc[i0][i1] * pow(inr, 8)
										- 8 * pd[i0][i1] * pow(inr, 10)
										+ inr * inr * inr * zz[i0][i1]);
							}

						vir -= rd * dphir;

						d3 = d0 * dphir;
						d4 = d1 * dphir;
						d5 = d2 * dphir;

						fc[i] += d3;
						fc[i + 1] += d4;
						fc[i + 2] += d5;
						fc[j] -= d3;
						fc[j + 1] -= d4;
						fc[j + 2] -= d5;

					}
				}

}

void dscuda_Force (){
	double zz2[2][2], center[3];
	int ii, jj;
	static n3_bak = 0;
	if (n3 != n3_bak) {
		if (n3_bak != 0)
			n3_bak = n3;
		}
	for (ii = 0; ii < 2; ii++)for (jj = 0; jj < 2; jj++)zz2[ii][jj] = zz[ii][jj];

	/////////////////////Here is DSCUDA func/////////
	LOGI("DSCUDA enable...........");

	 MR3calcnacl(n3,grape_flg,phi,&phir,iphi,&vir,s_num3,time_v,&md_time0,&md_time,
	 			&m_clock,md_step,&mtemp,tscale,&mpres,nden,s_num,w_num,rtemp,lq,
	 			cd,n3/3,atype,2,(double *)pol,(double *)sigm,
	 		    (double *)ipotro,(double *)pc,(double *)pd,
	 		    (double*)zz2,8,side[0],0,fc,
	 			hsq,a_mass,atype_mat,&ekin,vl,
	 			&xs,side);


}

/////////////////////////////////////////////////

void md_run(JNIEnv* env, jobject obj) {

		int i, j, k, c;
		int i0, i1, i2, i3, i4, i5;
		double d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12;

		double agv0, agv1, agv2;
		double ang0, ang1, ang2, ang3;
		int md_loop;

		double dphir;


	/////////////////////////MD_LOOP////////////////
		if (grape_flg == 0){
			for (md_loop = 0; md_loop < md_step; md_loop++) {

				m_clock++;


				for (i = 0; i < n3; i++) { /* update coordinations */
					if (atype[i / 3] <= 2 && atype[i / 3] != 8) {
						vl[i] = (vl[i] * (1 - xs) + fc[i]) / (1 + xs);
						cd[i] += vl[i];
					}
				}


				for (i = 0; i < n3; i += 3) {
					if (atype[i / 3] <= 2) {

						if (cd[i] < 0 || cd[i] > side[0])	vl[i] *= -1;
						if (cd[i + 1] < 0 || cd[i + 1] > side[1])	vl[i + 1] *= -1;
						if (cd[i + 2] < 0 || cd[i + 2] > side[2])	vl[i + 2] *= -1;
						} else {
						printf("atye[%d] is %d\n", i / 3, atype[i / 3]);
					}
				}

				/*****************  calculation of force  *********************/

				gettimeofday(&time_v, NULL );
				md_time0 = (time_v.tv_sec + time_v.tv_usec / 1000000.0);


				for (i = 0; i < 2; i++) {
					phi[i] = 0;
				}
				phir = 0;
				for (i = 0; i < n3; i++) {
					fc[i] = 0;
					iphi[i] = 0;
				}
				vir = 0;

				/*#if MDM == 0*/
				for (i = 0; i < s_num3; i++) {
					fc[i] = 0;
				}
				/////////////////////////Computing force////////////////

				ndk_Force();

				/////////////////////////////////////////////////////////

		#ifdef LAP_TIME
				gettimeofday(&time_v, NULL );
				md_time = (time_v.tv_sec + time_v.tv_usec / 1000000.0);
		#endif

		for (i = 0; i < n3; i++) {
			if (atype[i / 3] == 2)
				fc[i] *= hsq / (a_mass[2] + 2 * a_mass[3]);
			else if (atype[i / 3] == 0 || atype[i / 3] == 1)
				fc[i] *= hsq / a_mass[atype_mat[atype[i / 3]]];
		}

		for (i = 0; i < w_num3; i++)
			trq[i] *= hsq;

		ekin1 = 0;
		ekin2 = 0;
		for (i = 0; i < n3; i += 3) {
			ekin1 += (vl[i] * vl[i] + vl[i + 1] * vl[i + 1]
					+ vl[i + 2] * vl[i + 2]) * a_mass[atype_mat[atype[i / 3]]];
		}
		for (i = 0; i < w_num3; i += 3) {
			ekin2 += (moi[0] * agvph[i] * agvph[i]
					+ moi[1] * agvph[i + 1] * agvph[i + 1]
					+ moi[2] * agvph[i + 2] * agvph[i + 2]);
		}

		ekin1 /= hsq;
		ekin2 /= hsq;

		ekin = ekin1 + ekin2;

		mtemp = tscale * ekin;
		mpres = nden / 3. * (ekin - vir) / (s_num + w_num);
		xs += (mtemp - rtemp) / lq * hsq * .5;

}
}

		else if (grape_flg == 1)dscuda_Force();

		//else if (grape_flg == 2) Native_PosixThreads(env,obj);


///////////////////////////////////////end of MDLOPP
		//////////////////////////////////////////////

#if defined(SUBWIN) && defined(GL_ON)
		if ((b_clock % 10) == 0){
			if (p_count < DATA_NUM) {
				temp_data[p_count] = (int) (mtemp * epsv / kb);
				p_count++;
			} else {
				for (i = 0; i < DATA_NUM - 1; i++) {
					temp_data[i] = temp_data[i + 1];
				}
				temp_data[DATA_NUM - 1] = (int) (mtemp * epsv / kb);
			}

			temp_max = 0;
			for (i = 0; i < DATA_NUM; i++) {
				if (temp_data[i] > temp_max)
					temp_max = temp_data[i];
			}
			if (temp_ymax < temp_max) {
				temp_ymax = temp_max * 1.5;
			}
			if (temp_ymax > temp_max * 2 && temp_ymax / 2 > 2000) {
				temp_ymax /= 2;
			}

		}
#endif

	#ifdef GL_ON
		if (sc_flg != 1)

	#endif

		if (auto_flg == 1) {
	#ifdef GL_ON
			mouse_l = tt[b_clock].mouse[0];
			mouse_m = tt[b_clock].mouse[1];
			mouse_r = tt[b_clock].mouse[2];
			for (i = 0; i < 3; i++) {
				trans[i] += tt[b_clock].move[i];
				angle[i] = tt[b_clock].rot[i];
			}
	#endif
			if (sc_flg != 1) {

			}
			temp += tt[b_clock].temp;
			rtemp = temp / epsv * kb;
			for (i = 0; i < 16; i++)
				m_matrix[i] += tt[b_clock].matrix[i];

		}

		b_clock++;


/*
	LOGE("%4d %f %f %f %f %f %f\n",m_clock,mtemp*epsv/kb
	 ,(ekin/2.+phir)*erdp/(double)(s_num/2+w_num)
	 ,ekin/2.*erdp/(double)(s_num/2+w_num)
	 ,phir*erdp/(double)(s_num/2+w_num)
	 ,ekin1/(ekin1+ekin2), ekin2/(ekin1+ekin2));
*/
}


	//////////////////////////////////////////////Claret Part
///////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	void mygluPerspective( GLfloat fovy, GLfloat aspect,
			GLfloat zNear, GLfloat zFar )
	{
	   GLfloat xmin, xmax, ymin, ymax;
	   ymax = zNear * tan( fovy * M_PI / 360.0 );
	   ymin = -ymax;
	   xmin = ymin * aspect;
	   xmax = ymax * aspect;
	   glFrustumf( xmin, xmax, ymin, ymax, zNear, zFar );
	}

	void mygluLookAt( GLfloat eyex, GLfloat eyey, GLfloat eyez,
										GLfloat centerx, GLfloat centery, GLfloat centerz,
										GLfloat upx, GLfloat upy, GLfloat upz )
		{
		   GLfloat m[16];
		   GLfloat x[3], y[3], z[3];
		   GLfloat mag;
		   /* Make rotation matrix */
		   /* Z vector */
		   z[0] = eyex - centerx;
		   z[1] = eyey - centery;
		   z[2] = eyez - centerz;
		   mag = sqrt( z[0]*z[0] + z[1]*z[1] + z[2]*z[2] );
		   if (mag) {  /* mpichler, 19950515 */
		      z[0] /= mag;
		      z[1] /= mag;
		      z[2] /= mag;
		   }
		   /* Y vector */
		   y[0] = upx;
		   y[1] = upy;
		   y[2] = upz;
		   /* X vector = Y cross Z */
		   x[0] =  y[1]*z[2] - y[2]*z[1];
		   x[1] = -y[0]*z[2] + y[2]*z[0];
		   x[2] =  y[0]*z[1] - y[1]*z[0];
		   /* Recompute Y = Z cross X */
		   y[0] =  z[1]*x[2] - z[2]*x[1];
		   y[1] = -z[0]*x[2] + z[2]*x[0];
		   y[2] =  z[0]*x[1] - z[1]*x[0];
		   /* mpichler, 19950515 */
		   /* cross product gives area of parallelogram, which is < 1.0 for
		    * non-perpendicular unit-length vectors; so normalize x, y here
		    */
		   mag = sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] );
		   if (mag) {
		      x[0] /= mag;
		      x[1] /= mag;
		      x[2] /= mag;
		   }
		   mag = sqrt( y[0]*y[0] + y[1]*y[1] + y[2]*y[2] );
		   if (mag) {
		      y[0] /= mag;
		      y[1] /= mag;
		      y[2] /= mag;
		   }
		#define M(row,col)  m[col*4+row]
		   M(0,0) = x[0];  M(0,1) = x[1];  M(0,2) = x[2];  M(0,3) = 0.0;
		   M(1,0) = y[0];  M(1,1) = y[1];  M(1,2) = y[2];  M(1,3) = 0.0;
		   M(2,0) = z[0];  M(2,1) = z[1];  M(2,2) = z[2];  M(2,3) = 0.0;
		   M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
		#undef M
		   glMultMatrixf( m );
		   /* Translate Eye to Origin */
		   glTranslatef( -eyex, -eyey, -eyez );
}


void CircleTable (float **sint,float **cost,const int n){
    int i;
    /* Table size, the sign of n flips the circle direction */
    const int size = abs(n);
    /* Determine the angle between samples */
    const float angle = 2*M_PI/(float)( ( n == 0 ) ? 1 : n );
    /* Allocate memory for n samples, plus duplicate of first entry at the end */
    *sint = (float *) calloc(sizeof(float), size+1);
    *cost = (float *) calloc(sizeof(float), size+1);
    /* Bail out if memory allocation fails, fgError never returns */
    if (!(*sint) || !(*cost))
    {
        free(*sint);
        free(*cost);
        printf("Failed to allocate memory in fghCircleTable");
		exit(0);
    }
    /* Compute cos and sin around the circle */
    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;
    for (i=1; i<size; i++)
    {
        (*sint)[i] = sin(angle*i);
        (*cost)[i] = cos(angle*i);
    }
    /* Last sample is duplicate of the first */
    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void initClaretGL(){
			 // Initialize GL state.
	int i,j;

	  GLfloat mat_specular[] = {0.2, 0.2, 0.2, 1.0};
	  GLfloat mat_ambient[] = {0.1, 0.1, 0.1, 1.0};
	  GLfloat mat_shininess[] = {64.0};
	  GLfloat light_position[] = {1.0, 1.1, 1.2, 0.0};

	  float fm_matrix[16];
	  float fi_matrix[16];

#if 0
	  glShadeModel(GL_SMOOTH);
	/*  glShadeModel(GL_FLAT);*/
	  glLightfv(GL_LIGHT0, GL_SPECULAR, mat_specular);
	  glLightfv(GL_LIGHT0, GL_SHININESS, mat_shininess);
	  glLightfv(GL_LIGHT0, GL_AMBIENT, mat_ambient);
	  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
#endif

	  glMatrixMode(GL_MODELVIEW);

	  glGetFloatv(GL_MODELVIEW_MATRIX,m_matrix);
	  glGetFloatv(GL_MODELVIEW_MATRIX,i_matrix);

	  color_table[0][0] = 0.7;
	  color_table[0][1] = 0.38;
	  color_table[0][2] = 0.38;
	  color_table[0][3] = 1;

	  color_table[1][0] = 0.38;
	  color_table[1][1] = 0.55;
	  color_table[1][2] = 0.38;
	  color_table[1][3] = 1;

	  for(i = 0; i < 3; i++){
	    color_table[0][i] /= 2.0;
	    color_table[1][i] /= 2.0;
	  }

	  color_table[2][0] = 1;
	  color_table[2][1] = .4;
	  color_table[2][2] = 1;
	  color_table[2][3] = 1;

	  color_table[3][0] = 0;
	  color_table[3][1] = 0.8;
	  color_table[3][2] = 1;
	  color_table[3][3] = 1;

	  color_table[4][0] = 1;
	  color_table[4][1] = 1;
	  color_table[4][2] = 1;
	  color_table[4][3] = 1;

	  r_table[0] = 2.443/2;
	  r_table[1] = 3.487/2;
	  r_table[2] = 3.156/2;
	  r_table[3] = .7;
	  r_table[4] = .7;

	  for(i = 0; i < 3; i++)
	    trans0[i] = 0;
	  for(i = 0; i < 4; i++){
	    for(j = 0; j < 4; j++){
	      if(i == j){
		matrix0[i*4+j] = 1;
	      } else {
		matrix0[i*4+j] = 0;
	      }
	    }
	  }

}




void free_mem(){
	int i;
	free(nli);
	free(nig_num);
	free(nig_data);
	free(atype);
	free(cd);
	free(vl);
	free(fc);
	free(fcc);
	free(iphi);
	free(ang);
	free(agv);
	free(agvp);
	free(angh);
	free(agvh);
	free(trq);
	free(w_index);
	free(w_rindex);
	free(w_info);
	free(erfct);
	free(output);
	for (i = 0; i < VMAX; i++){
		free(vecn[i]);
		}

	    LOGI("Releasing Memory..........");
}


// Called from the app framework.
void appDeinit()
{
			LOGI("Quit..........");

}


static void gluPerspective(GLfloat fovy, GLfloat aspect,
                           GLfloat zNear, GLfloat zFar)
{
    GLfloat xmin, xmax, ymin, ymax;

    ymax = zNear * (GLfloat)tan(fovy * PI / 360);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustumx((GLfixed)(xmin * 65536), (GLfixed)(xmax * 65536),
               (GLfixed)(ymin * 65536), (GLfixed)(ymax * 65536),
               (GLfixed)(zNear * 65536), (GLfixed)(zFar * 65536));
}

static void reshape(int w, int h){
		glViewport(0, 0, w, h);

		//glClearColor((GLfixed)(clear_color),(GLfixed)(clear_color),(GLfixed)(clear_color),0.0);
		glClearColor(clear_color,clear_color,clear_color,0.0);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0, (float)w / (float)h, 0.5f, 800.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
}


GLuint LoadTextureRAW( const char * filename, int wrap )

{
  GLuint texture;

  int width, height;
  unsigned char* data;
  FILE * file;
  // open texture data
  file = fopen( filename, "rb" );
  if ( file == NULL ) {
  	LOGE("texture NOT loaded....");
  	return 0;
  }
  // allocate buffer
  width = 256;
  height = 256;
  data = malloc( width * height * 3 );
  // read texture data
  fread( data, width * height * 3, 1, file );
  fclose( file );
  // allocate a texture name
  glGenTextures( 1, &texture );
 // select our current texture
  glBindTexture( GL_TEXTURE_2D, texture );
 // select modulate to mix texture with color for shading
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  // when texture area is small, bilinear filter the closest MIP map
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );
  // when texture area is large, bilinear filter the first MIP map

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  // if wrap is true, the texture wraps over at the edges (repeat)
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
  // free buffer
  free( data );
  return texture;
}
// Called from the app framework.
/* The tick is current time in milliseconds, width and height
 * are the image dimensions to be rendered.
 */
static void display(){
		int which =0;
		float d0,d1,d2,d3,d4,d5;
	  float mag;
	  int i,j;
	  int i0;
	  char str_buf[256];
	  char str_buf2[256];
	  GLfloat particle_color[4];
	  GLfloat color[4];

	  ///////////////////////////////
	  /////////////////////////////////
	///////for Drawing with client;

#if 1  /////for color
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glCullFace(GL_BACK);
#endif


	  glPushMatrix();
	  d3 = atan((eye_width*which)/eye_len);
	  d1 = sin(d3)*eye_len;
	  d0 = cos(d3)*eye_len;
	  //gluLookAt(d0, d1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	  mygluLookAt(d0*3, d1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

#if 1
	//  glTranslated(trans[0], trans[1], trans[2]);
	  glTranslatef(trans[0], trans[1], trans[2]);


	  glPushMatrix();
	  glLoadIdentity();
	  glRotatef( angle[0],1.0,0.0,0.0);
	  glRotatef( angle[1],0.0,1.0,0.0);
	  glRotatef( angle[2],0.0,0.0,1.0);
	  //glMultMatrixd(m_matrix);
	  glMultMatrixf(m_matrix);

	  //glGetDoublev(GL_MODELVIEW_MATRIX, m_matrix);
	  glGetFloatv(GL_MODELVIEW_MATRIX, m_matrix);
	  glPopMatrix();


	  for(i = 0; i < 16; i++)
	    i_matrix[i] = m_matrix[i];
	  mat_inv((float(*)[4])i_matrix);

	  //glMultMatrixd(m_matrix);
	  glMultMatrixf(m_matrix);

	  //////Mouse Detection

	  	angle[0] = 0;
	  	if(mouse_l == 1 || mouse_m == 1 || mouse_r == 1){
	  	angle[1] = 0;
	  	angle[2] = 0;
	  	}
	  	if(ini_flg == 1){
	  	mouse_l = 0;
	  	ini_flg = 0;
	  	}

#endif

	  d2 = (i_matrix[0]*(1.0)+i_matrix[4]*(1.0)+i_matrix[8]*(1.0));
	  d3 = (i_matrix[1]*(1.0)+i_matrix[5]*(1.0)+i_matrix[9]*(1.0));
	  d4 = (i_matrix[2]*(1.0)+i_matrix[6]*(1.0)+i_matrix[10]*(1.0));

	  d0 = side0/2;
	  d1 = 0.3;
	  color[0] = d1; color[1] = d1; color[2] = d1; color[3] = 1.0;
	  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);
	  glLineWidth(2.0);
	  glPointSize(6.0);


	#if 1
	    ///////For Drawing with draw arrays
	   GLfloat line1[]={0,0,-d0,0,0,d0};
	   GLfloat Nline[]={d2,d3,d4};
	   GLfloat line2[]={0,-d0,0,0,d0,0};
	   GLfloat line3[]={-d0,0,0,d0,0,0};
	   GLfloat white[]={1.0,0.0,1.0,1.0};
	   GLfloat white2[]={1.0,0.5,1.0,1.0};
	   float side_s[3],side_e[3];

	   for(i = 0; i < 3; i++){
	       side_s[i] = -sideh[i];
	       side_e[i] = side[i]-sideh[i];
	   }

	 //////Coordinates for Cube
	   float cubevert1[]={  side_s[0],side_s[1],side_s[2],
			   	   	   	   side_s[0],side_e[1],side_s[2],
			   	   	   	   side_s[0],side_e[1],side_e[2],
			   	   	   	   side_s[0],side_s[1],side_e[2]
	   };

	   float cubevert2[]={ side_e[0],side_s[1],side_s[2],
			   	   	   	   side_e[0],side_e[1],side_s[2],
			   	   	   	   side_e[0],side_e[1],side_e[2],
			   	   	   	   side_e[0],side_s[1],side_e[2]
	   };

	   float cubebody[]={side_e[0],side_s[1],side_s[2],
			   side_s[0],side_s[1],side_s[2],
			   side_e[0],side_e[1],side_s[2],
			   side_s[0],side_e[1],side_s[2],
			   side_e[0],side_e[1],side_e[2],
			   side_s[0],side_e[1],side_e[2],
			   side_e[0],side_s[1],side_e[2],
			   side_s[0],side_s[1],side_e[2]
	   };


	   glEnableClientState(GL_VERTEX_ARRAY);
	   glEnableClientState(GL_NORMAL_ARRAY);
	   glEnableClientState(GL_COLOR_ARRAY);
	   glEnable(GL_COLOR_MATERIAL);

	/////Drawing the Cross 3D Axis X,Y,Z
	    glVertexPointer(3,GL_FLOAT,0,line1);
	    glNormalPointer(GL_FLOAT,0,Nline);
	    glColorPointer(4,GL_FLOAT,0,white);
	    glDrawArrays(GL_LINES,0,2);

	    glVertexPointer(3,GL_FLOAT,0,line2);
	    glNormalPointer(GL_FLOAT,0,Nline);
	    glColorPointer(4,GL_FLOAT,0,white);
	    glDrawArrays(GL_LINES,0,2);

	    glVertexPointer(3,GL_FLOAT,0,line3);
	    glNormalPointer(GL_FLOAT,0,Nline);
	    glColorPointer(4,GL_FLOAT,0,white);
	    glDrawArrays(GL_LINES,0,2);

	 ///////Drawing the Cube lines

	    glVertexPointer(3,GL_FLOAT,0,cubevert1);
	    glNormalPointer(GL_FLOAT,0,Nline);
	    glColorPointer(4,GL_FLOAT,0,white);
	    glDrawArrays(GL_LINE_LOOP,0,4);

	    glVertexPointer(3,GL_FLOAT,0,cubevert2);
	    glNormalPointer(GL_FLOAT,0,Nline);
	    glColorPointer(4,GL_FLOAT,0,white);
	    glDrawArrays(GL_LINE_LOOP,0,4);

	    glVertexPointer(3,GL_FLOAT,0,cubebody);
	    glNormalPointer(GL_FLOAT,0,Nline);
	    glColorPointer(4,GL_FLOAT,0,white2);
	    glDrawArrays(GL_LINES,0,8);

	#endif


	///////////////////////////////////Drawing with DrawElements
#if 1
	    GLuint buf[3];
			glGenBuffers(3, buf);

			int n = n3/3;
			int q,m,l=3,k,p,t=0,r=0,slices =ditail,stacks=ditail/2;
			float z0,z1,r0,r1,radios;

			float *f_pol,*f_pol_n,*f_clr_a,*f_clr_b;
			float *sint1,*cost1;
			float *sint2,*cost2;

			int cuad_mem    =(((ditail/2)-2)*ditail*6*3);//+1; por si ditail=5  =>0
			int pol_mem 	= cuad_mem + (ditail*3*3*2);
			int pol_size 	=(ditail*3*2)+(cuad_mem/3);

			unsigned int size 		= (n*pol_mem)*sizeof(float);
			unsigned int size_color = (n*pol_size*4)*sizeof(float);


			f_pol    = (float*)malloc(n*pol_mem*sizeof(float));
			f_pol_n  = (float*)malloc(n*pol_mem*sizeof(float));
			f_clr_a	 = (float*)malloc(n*pol_size*4*sizeof(float));
			f_clr_b	 = (float*)malloc(n*pol_size*4*sizeof(float));

			/* Pre-computed circle */


			CircleTable(&sint1,&cost1,-slices);
			CircleTable(&sint2,&cost2,stacks*2);

		//////Mapping The circle////////////////////////////////////////////////////
		#if 1

			for(i=0; i<n3;i+=3){
				 if(drow_flg[atype_mat[atype[i/3]]] == 1){
					 /////////////////////Compute Color
					 for(q=0;q<pol_size;q++){
					d0 = (vl[i]*vl[i]+vl[i+1]*vl[i+1]+vl[i+2]*vl[i+2])*500;
					*(f_clr_a+0+r+q*4) = color_table[atype_mat[atype[i/3]]][0]+d0;
					*(f_clr_a+1+r+q*4) = color_table[atype_mat[atype[i/3]]][1]+d0/3;
					*(f_clr_a+2+r+q*4) = color_table[atype_mat[atype[i/3]]][2]+d0/3;
					*(f_clr_a+3+r+q*4) = color_table[atype_mat[atype[i/3]]][3];

					 }

					radios=radius*r_table[atype_mat[atype[i/3]]];
					///////Compute one Circle/////////////////////////
							z0 = 1.0f;
									z1 = cost2[(stacks>0)?1:0];
									r0 = 0.0f;
									r1 = sint2[(stacks>0)?1:0];

									for(m=0;m<slices;m++){
													*(f_pol+t+(9*m))     =cd[i]-sideh[0];
													*(f_pol+t+1+(9*m))   =cd[i+1]-sideh[1];
													*(f_pol+t+2+(9*m))   =cd[i+2]-sideh[2]+radios;
													*(f_pol_n+t+(9*m))    =0;
													*(f_pol_n+t+1+(9*m))  =0;
													*(f_pol_n+t+2+(9*m))  =1;
											}

											l=3;

											for (j=slices; j>0; j--){
													*(f_pol+t+l)     =(cd[i]-sideh[0])+(cost1[j]*r1*radios);
													*(f_pol+t+l+1)   =(cd[i+1]-sideh[1])+(sint1[j]*r1*radios);
													*(f_pol+t+l+2)   =(cd[i+2]-sideh[2])+ (z1*radios);
													*(f_pol_n+t+l)   =cost1[j]*r1;
													*(f_pol_n+t+l+1) =sint1[j]*r1;
													*(f_pol_n+t+l+2) =z1;

													*(f_pol+t+l+3)   =(cd[i]-sideh[0])  + (cost1[j-1]*r1*radios);
													*(f_pol+t+l+4)   =(cd[i+1]-sideh[1])+ (sint1[j-1]*r1*radios);
													*(f_pol+t+l+5)   =(cd[i+2]-sideh[2])+ (z1*radios);
													*(f_pol_n+t+l+3) =cost1[j-1]*r1;
													*(f_pol_n+t+l+4) =sint1[j-1]*r1;
													*(f_pol_n+t+l+5) =z1;

													l+=9;
											}
					/////////////////
										 l-=3;
											for( k=1; k<stacks-1; k++ ){
													z0 = z1; z1 = cost2[k+1];
													r0 = r1; r1 = sint2[k+1];

													p=0;
													for(j=0; j<slices; j++){
														 //////////////////First Triangle////////////////////////////////
															*(f_pol+t+l+p)     = (cd[i]-sideh[0] ) +(cost1[j]*r1*radios);
															*(f_pol+t+l+p+1)   = (cd[i+1]-sideh[1])+(sint1[j]*r1*radios);
															*(f_pol+t+l+p+2)   = (cd[i+2]-sideh[2])+(z1*radios);
															*(f_pol_n+t+l+p)   = cost1[j]*r1;
															*(f_pol_n+t+l+p+1) = sint1[j]*r1;
															*(f_pol_n+t+l+p+2) = z1;

															*(f_pol+t+l+p+3)   = (cd[i]-sideh[0])  +(cost1[j]*r0*radios);
															*(f_pol+t+l+p+4)   = (cd[i+1]-sideh[1])+(sint1[j]*r0*radios);
															*(f_pol+t+l+p+5)   = (cd[i+2]-sideh[2])+(z0*radios);
															*(f_pol_n+t+l+p+3) = cost1[j]*r0;
															*(f_pol_n+t+l+p+4) = sint1[j]*r0;
															*(f_pol_n+t+l+p+5) = z0;

															*(f_pol+t+l+p+6)   = (cd[i]-sideh[0])  +(cost1[j+1]*r1*radios);
															*(f_pol+t+l+p+7)   = (cd[i+1]-sideh[1])+(sint1[j+1]*r1*radios);
															*(f_pol+t+l+p+8)   = (cd[i+2]-sideh[2])+(z1*radios);
															*(f_pol_n+t+l+p+6) = cost1[j+1]*r1;
															*(f_pol_n+t+l+p+7) = sint1[j+1]*r1;
															*(f_pol_n+t+l+p+8) = z1;

															/////////////////////Second Triangle//////////////////////////////

															*(f_pol+t+l+p+9)   = *(f_pol+t+l+p+6);
															*(f_pol+t+l+p+10)  = *(f_pol+t+l+p+7);
															*(f_pol+t+l+p+11)  = *(f_pol+t+l+p+8);
															*(f_pol_n+t+l+p+9) = *(f_pol_n+t+l+p+6);
															*(f_pol_n+t+l+p+10)= *(f_pol_n+t+l+p+7);
															*(f_pol_n+t+l+p+11)= *(f_pol_n+t+l+p+8);

															*(f_pol+t+l+p+12)   = *(f_pol+t+l+p+3);
															*(f_pol+t+l+p+13)   = *(f_pol+t+l+p+4);
															*(f_pol+t+l+p+14)   = *(f_pol+t+l+p+5);
															*(f_pol_n+t+l+p+12) = *(f_pol_n+t+l+p+3);
															*(f_pol_n+t+l+p+13) = *(f_pol_n+t+l+p+4);
															*(f_pol_n+t+l+p+14) = *(f_pol_n+t+l+p+5);

															*(f_pol+t+l+p+15)  =(cd[i]-sideh[0] ) +(cost1[j+1]*r0*radios);
															*(f_pol+t+l+p+16)  =(cd[i+1]-sideh[1])+(sint1[j+1]*r0*radios);
															*(f_pol+t+l+p+17)  =(cd[i+2]-sideh[2])+(z0*radios);
															*(f_pol_n+t+l+p+15)=cost1[j+1]*r0;
															*(f_pol_n+t+l+p+16)=sint1[j+1]*r0;
															*(f_pol_n+t+l+p+17)=z0;

															p+=18;
													}
											l+=(slices)*6*3;
											}
					////////////////////
											z0 = z1;
											r0 = r1;

											for(m=0;m<slices;m++){
									*(f_pol+t+l+(9*m))     = cd[i]-sideh[0];
									*(f_pol+t+l+1+(9*m))   = cd[i+1]-sideh[1];
									*(f_pol+t+l+2+(9*m))   = cd[i+2]-sideh[2]-radios;
									*(f_pol_n+t+l+(9*m))   = 0;
									*(f_pol_n+t+l+1+(9*m)) = 0;
									*(f_pol_n+t+l+2+(9*m)) = -1;
											}

											p=3;
											for (j=0; j<slices; j++){
													*(f_pol+t+l+p)     = (cd[i]-sideh[0])  + (cost1[j]*r0*radios);
													*(f_pol+t+l+p+1)   = (cd[i+1]-sideh[1])+ (sint1[j]*r0*radios);
													*(f_pol+t+l+p+2)   = (cd[i+2]-sideh[2])+ (z0*radios);
													*(f_pol_n+t+l+p)   = cost1[j]*r0;
													*(f_pol_n+t+l+p+1) = sint1[j]*r0;
													*(f_pol_n+t+l+p+2) = z0;

													*(f_pol+t+l+p+3)   = (cd[i]-sideh[0] ) +(cost1[j+1]*r0*radios);
													*(f_pol+t+l+p+4)   = (cd[i+1]-sideh[1])+(sint1[j+1]*r0*radios);
													*(f_pol+t+l+p+5)   = (cd[i+2]-sideh[2])+(z0*radios);
													*(f_pol_n+t+l+p+3) = cost1[j+1]*r0;
													*(f_pol_n+t+l+p+4) = sint1[j+1]*r0;
													*(f_pol_n+t+l+p+5) = z0;

													p+=9;
											}

						 }
						 t+=pol_mem;
						 r+=pol_size*4;
			}

		//////////////////////////Prepare Circle mapped buffers

			glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
			glBufferData(GL_ARRAY_BUFFER,size,f_pol, GL_DYNAMIC_DRAW);
			glVertexPointer(3, GL_FLOAT, 0, 0);

			glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
			glBufferData(GL_ARRAY_BUFFER, size,f_pol_n, GL_DYNAMIC_DRAW);
			glNormalPointer(GL_FLOAT,0,0);

			glBindBuffer(GL_ARRAY_BUFFER, buf[2]);
			glBufferData(GL_ARRAY_BUFFER,size_color,f_clr_a, GL_DYNAMIC_DRAW);
			glColorPointer(4,GL_FLOAT,0,0);

			glDrawArrays(GL_TRIANGLES,0,pol_size*n);
			//glDrawArrays(GL_LINES,0,pol_size*n);



		#endif
			///////////////////////END of Drawing circle//////////////////////////

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			glDisable(GL_COLOR_MATERIAL);

			glDeleteBuffers(3, buf);

			free(f_clr_a);
			free(f_clr_b);
			free(f_pol_n);
			free(f_pol);
			free(sint1);
			free(cost1);
			free(sint2);
			free(cost2);

#endif

		glPopMatrix();
/////////////////////////////////
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);

}

void touch(int x, int y,int multi)
{

  if(multi==1)
  {
  	mpos[0] = x;
  	mpos[1] = y;
  	mouse_l = 1;
  }
  else if (multi == 2)
  {
  	mpos[0] = x;
  	mpos[1] = y;
  	mouse_r = 1;
  }
  else if (multi == 3)
  {
  	mpos[0] = x;
  	mpos[1] = y;
  	mouse_m = 1;
  }
  else if (multi == 0)
  {
  	mouse_l = 0;
  	mouse_r = 0;
  	mouse_m = 0;
  }

}
void motion(int x, int y)
{


  float d0;
  float len = 10;
  len = eye_len;

  if(mouse_m == 1){
    trans[1] += (float)(x-mpos[0])*len*.001;
    trans[2] -= (float)(y-mpos[1])*len*.001;
  } else if(mouse_r == 1){
    trans[0] -= (float)(y-mpos[1])*len/150;
    angle[0] =  (float)(x-mpos[0])*0.2;
  } else if(mouse_l == 1){
    d0 = len/50;
    if(d0 > 1.0) d0 = 1.0;
    angle[1] = (float)(y-mpos[1])*d0;
    angle[2] = (float)(x-mpos[0])*d0;
  }
  if(mouse_l == 1 || mouse_m == 1 || mouse_r == 1){
    mpos[0] = x;
    mpos[1] = y;
  }

}

//void appRender(long tick, int width, int height)
float * appRender(JNIEnv* env, jobject obj,int width,int height, int dscuforce, int glon)
{
     // Prepare OpenGL ES for rendering of the frame.

		grape_flg = dscuforce;
		reshape(width,height);

		if (glon == 1) md_run(env,obj);

    output[0] = (float)n1*(float)n1*78/(md_time-md_time0)*1e-9;
    display();

    ////////////Measure 1 Frame
    gettimeofday(&time_v,NULL);
    disp_time0 	= disp_time;
    disp_time 	= (time_v.tv_sec + time_v.tv_usec / 1000000.0);
    /////////////////////////////

    output[1] = (disp_time-disp_time0);
    output[2] = n1;
    output[3] = mtemp*epsv/kb,delt*m_clock;
    return output;

}

// Called from the app framework.

void appInit()
{
	if(gInitiated==0){
		np = 4;
		temp = 300;
		sub_x = 1.5;
		sub_y = 1.5;
		temp_ymax = 2000;

		initClaretGL();
		init_MD();
		keep_mem(S_NUM_MAX,W_NUM_MAX*w_site);
		set_cd(1);
		gInitiated=1;
	}
	else{

	}

 //   LOGE("OpenGL Version: %s",glGetString(GL_VERSION));
		LOGI("App Init.........");

}
