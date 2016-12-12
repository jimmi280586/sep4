import java.io.IOException;

import dk.thibaut.serial.SerialPort;

public class SerialConnectionReader {
	private int readBufferSize = 1024;
	private SerialPort serialPort;

	public SerialConnectionReader(SerialPort serialPort) {
		this.serialPort = serialPort;
	}

	public void printMSG() {
		try {
			System.out.println("msg");
			byte[] readBuffer = read();//somthing wrong
			System.out.println("  Recieved bytes: " + new String(readBuffer));
		} catch (Exception e) {
			System.out.println("Error in read");
			e.printStackTrace();
		}

	}

	public byte[] read() throws InterruptedException {
		byte[] readBuffer = new byte[readBufferSize];
		int numberOfBytes = 0;
		try {
			while (numberOfBytes == 0) {
				numberOfBytes = serialPort.getInputStream().read(readBuffer);
				Thread.sleep(10);
			}
			System.out.println("read number of bytes: " + numberOfBytes);
		} catch (IOException e) {
			System.err.println("Failed getting or reading input stream from " + serialPort.getName());
			e.printStackTrace();
		}

		byte[] msg = new byte[numberOfBytes];
		for (int i = 0; i < numberOfBytes; i++) {
			msg[i] = readBuffer[i];
		}

		return msg;
	}

}
