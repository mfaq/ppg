public class MsgBuf {

	public MsgBuf(int cap) {
		data = new int[cap];
		head = 0;
		tail = 0;
		qsize = 0;
	}
	
	public synchronized void send(int msg) {
		while (qsize == data.length) {
			try {
				wait();
			} catch(InterruptedException e) {}
		}
		data[tail++] = msg;
		if (tail == data.length)
			tail = 0;
		
		qsize++;
		notifyAll();
	}
	
	public synchronized int recv() {
		while (qsize == 0) {
			try {
				wait();
			} catch (InterruptedException e) {}
		}
		int msg = data[head++];
		if (head == data.length)
			head = 0;
		
		qsize--;
		notifyAll();
		
		return msg;
	}
	
	private int[] data;
	private int qsize;
	private int head;
	private int tail;

}
