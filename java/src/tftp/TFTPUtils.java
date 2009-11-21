package tftp;

/**
 * Class for handling common events/actions in application
 * 
 * @author Vilius
 *
 */

public class TFTPUtils {

	public static void puts(String s) {
	 
		System.out.println(s);
		
	}
	
	public static void FatalError(String msg) {
		
		System.err.println("Fatal error: " + msg);
		System.exit(1);
		
	}
	
}
