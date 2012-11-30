public class SortWithThreads {

	public static void oddEvenTransSort(int[] array) {
		int n = array.length;

		MsgBuf[] mq = new MsgBuf[n];
		Proc[] procs = new Proc[n];
		Thread[] threads = new Thread[n];

		for (int i = 0; i < n; i++) {
			mq[i] = new MsgBuf(1);
			procs[i] = new Proc(array, i, mq);
			threads[i] = new Thread(procs[i]);
		}

		for (Thread t : threads)
			t.start();

		try {
			for (Thread t : threads)
				t.join();
		} catch (InterruptedException e) {
		}

	}

	public static void main(String[] args) {
		int[] a = { 5, 4, 1, 3, 6, 7 };
		for (int v : a)
			System.out.print(v + " ");
		System.out.print("\n");

		oddEvenTransSort(a);

		for (int v : a)
			System.out.print(v + " ");

		System.out.print("\n");
	}

}
