package com.edgar.claret;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.edgar.claret.GLText;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.graphics.Point;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v4.view.MotionEventCompat;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.Window;
import android.widget.FrameLayout;
import android.widget.Toast;
import android.widget.ViewSwitcher;
import android.opengl.GLU;



public class ClaretActivity extends Activity {
	
		
	 @Override
	    protected void onCreate(Bundle savedInstanceState) {
	        requestWindowFeature(Window.FEATURE_NO_TITLE);
		 	super.onCreate(savedInstanceState);
		 	//setContentView(R.layout.activity_claret);
	        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
	       	       			
	        mGLView = new DemoGLSurfaceView(this);
	        setContentView(mGLView);
	       
	      	       
	        
	    }

	    @Override
	    protected void onPause() {
	        super.onPause();
	        mGLView.onPause();
	    }

	    @Override
	    protected void onResume() {
	        super.onResume();
	        mGLView.onResume();
	    }
	    
	    @Override
		protected void onDestroy() {
			// Free the native resources
			nativeFree();
			
			super.onDestroy();
		}
	    

	    private GLSurfaceView mGLView;
	   
	    	
	    static {
	        System.loadLibrary("claret");
	    }
	    
	    public void calcnacl(String text){
	    
	    }
	    
	    public native void nativeFree();
		
	    

}

/////////////////////////////////////////////////////////////

class DemoGLSurfaceView extends GLSurfaceView {
	public static int accelerator 	= 0; // 0 = Tablet CPU 1 = DSCUDA
	public static int glon 			= 1; //Render Flag... Skip md_run() = -1
	public static float [] output  	= new float [5]; // [0]=glfops,[1]=frames,[2]=#particles,
													// [3]=tmp,[4]=aux..
    public DemoGLSurfaceView(Context context) {
        super(context);
        mRenderer = new DemoRenderer(context);
                 
        setRenderer(mRenderer);
        
        
        
    }

   
	public boolean onTouchEvent(final MotionEvent event) {
       
		int action = MotionEventCompat.getActionMasked(event);
		int index  = MotionEventCompat.getActionIndex(event);
    	
    	if(event.getPointerCount()==1){
    		if(MotionEvent.ACTION_DOWN == action){
    			Log.d("debug java", "Single Touch  X= "+MotionEventCompat.getX(event, index)+" Y= "+MotionEventCompat.getY(event, index));
    			
    			
    			if(event.getX()>200.0f && event.getX()<=1000){
    				nativeTogglePauseResume(event.getX(), event.getY(),1);
    			}
    			else if(event.getX()>1000){
    				if(event.getY()<233.0f)glon*=-1;
    				
    			}
    			else{
    				if(event.getY()<233.0f)accelerator=1;
    				else if (event.getY()>=233 && event.getY()<467 )accelerator=0;
    				else accelerator=0;
    			}
    		}
    		
    		if(MotionEvent.ACTION_MOVE == action){
    			Log.e("debug java", "Single Touch  X= "+MotionEventCompat.getX(event, index)+" Y= "+MotionEventCompat.getY(event, index));
    			nativeMotion(event.getX(), event.getY());
    		}
    		
    		if(MotionEvent.ACTION_UP == action){
    			Log.i("debug java", "Single Touch  X= "+MotionEventCompat.getX(event, index)+" Y= "+MotionEventCompat.getY(event, index));
    			nativeTogglePauseResume(event.getX(), event.getY(),0);
    		}

    	}
    	
    	if (event.getPointerCount()==2){
    		if(MotionEvent.ACTION_POINTER_DOWN == action){
	    		Log.d("debug java", "Multi Touch 2  X= "+MotionEventCompat.getX(event, index)+" Y= "+MotionEventCompat.getY(event, index));
	    		nativeTogglePauseResume(event.getX(), event.getY(),2);
    		}
    		
    		if(MotionEvent.ACTION_MOVE == action){
    			Log.e("debug java", "Multi Touch 2  X= "+MotionEventCompat.getX(event, index)+" Y= "+MotionEventCompat.getY(event, index));
    			nativeMotion(event.getX(), event.getY());
    		}
    		
    		
    		if(MotionEvent.ACTION_POINTER_UP == action){
    			Log.i("debug java", "Multi Touch 2  X= "+MotionEventCompat.getX(event, index)+" Y= "+MotionEventCompat.getY(event, index));
    			nativeMotion(event.getX(), event.getY());
    			nativeTogglePauseResume(event.getX(), event.getY(),0);
    		}
    		    		
       	}
    	if (event.getPointerCount()==3){
    		if(MotionEvent.ACTION_POINTER_DOWN == action){
	    		Log.d("debug java", "Multi Touch 3  X= "+MotionEventCompat.getX(event, index)+" Y= "+MotionEventCompat.getY(event, index));
	    		nativeTogglePauseResume(event.getX(), event.getY(),3);
    		}
    		
    		if(MotionEvent.ACTION_MOVE == action){
    			Log.e("debug java", "Multi Touch 3  X= "+MotionEventCompat.getX(event, index)+" Y= "+MotionEventCompat.getY(event, index));
    			nativeMotion(event.getX(), event.getY());
    		}
    		
    		if(MotionEvent.ACTION_POINTER_UP == action){
    			Log.i("debug java", "Multi Touch 3  X= "+MotionEventCompat.getX(event, index)+" Y= "+MotionEventCompat.getY(event, index));
    			nativeTogglePauseResume(event.getX(), event.getY(),0);
    		}
    		
    	}
        
        return true;
    }

   @Override
    public void onPause() {
        super.onPause();
        nativePause();
    }

   @Override
    public void onResume() {
        super.onResume();
        nativeResume();
    }

    DemoRenderer mRenderer;

    private static native void nativePause();
    private static native void nativeResume();
    private static native void nativeTogglePauseResume(float posx, float posy, int ind);
    private static native void nativeMotion(float posx, float posy);
}
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class DemoRenderer implements GLSurfaceView.Renderer {
	////////////Fonts
	private GLText glText;                             // A GLText Instance
	private Context context;                           // Context (from Activity)
		
	public DemoRenderer(Context context)  {
		super();
	    this.context = context;                         // Save Specified Context
	}
	
/////////////////////////////	  	
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	//////for Fonts
    	//gl.glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glText = new GLText( gl, context.getAssets() );

        // Load the font from file (set size + padding), creates the texture
        // NOTE: after a successful call to this the font is ready for rendering!
        glText.load( "Roboto-Light.ttf", 24, 2, 2 );  // Create Font (Height: 14 Pixels / X+Y Padding 2 Pixels)
    	///////////////
        nativeInit();
              
        
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
        nativeResize(w, h);
    }

    public void Text (GL10 gl){
    	int w=1280,h=700;
    	
    	String [] data = new String [5];
    	data[0] =String.format("%.3f",DemoGLSurfaceView.output[0])+"Gflops";
    	data[1] =String.format("%.3f",1.0f/DemoGLSurfaceView.output[1])+"Frm/s";
    	data[2] =String.format("%.1f",DemoGLSurfaceView.output[2])+" Bodies";
    	data[3] =String.format("%.3f",DemoGLSurfaceView.output[3])+" K";
/////////////////////////////////////////////////////////////////////////
    	aspect =  (float) w / (float) h;
    	camZ = h / 2 / (float) Math.tan(fov_radians / 2);

    	gl.glPushMatrix();
    	gl.glViewport(0, 0, w, h); // Reset The Current Viewport
    	gl.glMatrixMode(GL10.GL_PROJECTION); // Select The Projection Matrix
	   	gl.glLoadIdentity(); // Reset The Projection Matrix
	
	  //Calculate The Aspect Ratio Of The Window	  
    	GLU.gluPerspective(gl, fov_degrees, aspect, camZ / 10, camZ * 10);
    	GLU.gluLookAt(gl, 0, 0, camZ, 0, 0, 0, 0, 1, 0); // move camera back
    	gl.glMatrixMode(GL10.GL_MODELVIEW); // Select The Modelview Matrix
    	gl.glLoadIdentity(); // Reset The Modelview Matrix
	  	gl.glMatrixMode( GL10.GL_MODELVIEW );  
	   	gl.glLoadIdentity();
	   	              
		gl.glEnable( GL10.GL_TEXTURE_2D );              // Enable Texture Mapping
		gl.glEnable( GL10.GL_BLEND );                   // Enable Alpha Blend
		gl.glBlendFunc( GL10.GL_SRC_ALPHA, GL10.GL_ONE_MINUS_SRC_ALPHA );  // Set Alpha Blend Function
//////////////////////////////////////////////////////////////////////////////////	
		
		//////////Display information about system Gflops,tmps,frm etc..
		glText.begin( 0.2f, 1.0f, 0.1f, 1.0f );         // Begin Text Rendering (Set Color WHITE)
		glText.draw( data[0], 450.0f, -338.0f, 0.0f );          // Draw Test String
		glText.end();                                   // End Text Rendering
		
		glText.begin( 0.2f, 1.0f, 0.1f, 1.0f );         // Begin Text Rendering (Set Color WHITE)
		glText.draw( data[1], 450.0f, -318.0f, 0.0f );          // Draw Test String
		glText.end();
		
		glText.begin( 0.2f, 1.0f, 0.1f, 1.0f );         // Begin Text Rendering (Set Color WHITE)
		glText.draw( data[2], 450.0f, -298.0f, 0.0f );          // Draw Test String
		glText.end();
		
		glText.begin( 0.2f, 1.0f, 0.1f, 1.0f );         // Begin Text Rendering (Set Color WHITE)
		glText.draw( data[3], 450.0f, -278.0f, 0.0f );          // Draw Test String
		glText.end(); 
		////////////////////////////////////////////////////////////////////
		
		glText.begin( 0.2f, 1.0f, 0.1f, 1.0f );         // Begin Text Rendering (Set Color WHITE)
		glText.draw( "DSCUDA", -590.0f, 280.0f, 0.0f );          // Draw Test String
		glText.end();
	
		if(DemoGLSurfaceView.accelerator == 1)glText.begin( 1.0f, 0.0f, 0.0f, 1.0f );         // Begin Text Rendering (Set Color WHITE)
		else glText.begin( 0.2f, 1.0f, 0.1f, 1.0f ); 
		glText.draw( "on", -565.0f, 250.0f, 0.0f );          // Draw Test String
		glText.end();
		
		glText.begin( 0.2f, 1.0f, 0.1f, 1.0f );         // Begin Text Rendering (Set Color WHITE)
		glText.draw( "RenderOnly", 505.0f, 280.0f, 0.0f );          // Draw Test String
		glText.end();
	
		if(DemoGLSurfaceView.glon == 1)glText.begin( 1.0f, 0.0f, 0.0f, 1.0f );         // Begin Text Rendering (Set Color WHITE)
		else glText.begin( 0.2f, 1.0f, 0.1f, 1.0f ); 
		glText.draw( "on", 550.0f, 250.0f, 0.0f );          // Draw Test String
		glText.end();
		
		glText.begin( 0.2f, 1.0f, 0.1f, 1.0f );         // Begin Text Rendering (Set Color WHITE)
		glText.draw( "NDK", -580.0f, -290.0f, 0.0f );          // Draw Test String
		glText.end();
		
		if(DemoGLSurfaceView.accelerator == 0)glText.begin( 1.0f, 0.0f, 0.0f, 1.0f );
		else glText.begin( 0.2f, 1.0f, 0.1f, 1.0f ); 
		glText.draw( "On", -570.0f, -320.0f, 0.0f );          // Draw Test String
		glText.end();
		
		 // disable texture + alpha
		gl.glDisable( GL10.GL_BLEND );                  // Disable Alpha Blend
		gl.glDisable( GL10.GL_TEXTURE_2D );             // Disable Texture Mapping
		
		gl.glPopMatrix();
         /////////////////
    }
    
    public void onDrawFrame(GL10 gl) {
    	DemoGLSurfaceView.output = nativeRender(DemoGLSurfaceView.accelerator,
    								DemoGLSurfaceView.glon);
    	Text(gl);
    	
    	
    	//Log.i("Debug java", "Gflops : "+DemoGLSurfaceView.output[0]);
    	//Log.i("Debug java", "Frm : "+1.0f/DemoGLSurfaceView.output[1]);
    	//Log.i("Debug java", "NParticles : "+DemoGLSurfaceView.output[2]);
    	//Log.i("Debug java", "TMP : "+DemoGLSurfaceView.output[3]);
    	//if (DemoGLSurfaceView.on == 1)Log.e("Debug java", "DSCUDA is : "+DemoGLSurfaceView.on);
    	//else Log.i("Debug java", "DSCUDA is : "+DemoGLSurfaceView.on);
           
    }
    
    
    public static float fov_degrees =  45f;
    public static float fov_radians =  fov_degrees / 180 * (float) Math.PI;
    public static float aspect;
    public static float camZ;

    
    private static native void nativeInit();
    private static native void nativeResize(int w, int h);
    private static native float[] nativeRender(int x, int glon);
    private static native void nativeDone();
}

