package tftpclient;

import tftp.TFTPUtils;
 
import jargs.gnu.CmdLineParser;

public class TFTPClientUI {
	
	private static void printUsage() {
        System.err.println(
        		"\n" +
        		"Usage: TFTP -o host [-u] -s source [-d destination] [-p port] [-h]\n" +
        		" -h --help Prints this help" +
        		" -o --open Specifies the server ip address" +
        		" -u --upload Upload file to server" +
        		" -s --source Specifies source file name" +
        		" -d --destination Specifies destination file name" +
        		" -p --port Specifies the port number to be used\n");
    }
 

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		
		CmdLineParser parser = new CmdLineParser();
        CmdLineParser.Option opt_help 	= parser.addBooleanOption('h', "help");
        CmdLineParser.Option opt_host 	= parser.addStringOption('o', "open");
        CmdLineParser.Option opt_upload = parser.addBooleanOption('u', "upload");
        CmdLineParser.Option opt_source = parser.addStringOption('s', "source");
        CmdLineParser.Option opt_dest 	= parser.addStringOption('d', "destination");
        CmdLineParser.Option opt_port 	= parser.addIntegerOption('p', "port");
 
        try {
            parser.parse(args);
        }
        catch (CmdLineParser.OptionException e) {
            System.err.println(e.getMessage());
            printUsage();
            System.exit(2);
        }
 
        Boolean help = (Boolean)parser.getOptionValue(opt_help, Boolean.FALSE);
        
        if (help) {
        	printUsage();
        	System.exit(2);
        }
        
        String source = (String)parser.getOptionValue(opt_source, new String(""));
        if (source == "") {
        	printUsage();
        	System.exit(2);
        }
        String dest = (String)parser.getOptionValue(opt_dest, new String(source));
        String host = (String)parser.getOptionValue(opt_host, new String("127.0.0.1"));
        
        int port = (Integer)parser.getOptionValue(opt_port, new Integer(5555));
        
        Boolean upload = (Boolean)parser.getOptionValue(opt_upload, Boolean.FALSE);
        
		TFTPClient client = new TFTPClient(port, host);
		
		if (upload) {
			
			TFTPUtils.puts("Sending file to server: " + source);
		
			client.sendFile(source, dest);
			
		} else {
			
			TFTPUtils.puts("Getting file from server: " + source);
			
			client.getFile(source, dest);
			
		}
		
	}

}
