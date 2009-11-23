package tftpclient;

public class TFTPClientUI {

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		TFTPClient client = new TFTPClient(5555, "127.0.0.1");
		
		client.sendFile("test");
		
	}

}
