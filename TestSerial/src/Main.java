public class Main {

	SerialConnection serialConnection;

	public Main() throws InterruptedException {
		System.out.println("Befor connection");
		//Finde the right COM port in devices
		serialConnection = new SerialConnection("COM3");
		System.out.println("Befor reader connection");
		SerialConnectionReader serialConnectionReader = serialConnection.getSerialConnectionReader();

		serialConnection.write("A".getBytes());
		// Thread.sleep(500);

		serialConnection.write("B".getBytes());
		// Thread.sleep(1000);

		serialConnection.write("C".getBytes());
		// Thread.sleep(1000);

		serialConnection.write("D".getBytes());
		//Thread.sleep(1000);
		System.out.println("you are here");
		serialConnectionReader.printMSG();
		
		Thread.sleep(3000);
		serialConnection.write("X".getBytes());
		serialConnectionReader.printMSG();
		
		
		serialConnection.close();
		System.out.println("Goodby");
		System.exit(0);

	}

	public static void main(String[] args) throws InterruptedException {
		new Main();

	}

}
