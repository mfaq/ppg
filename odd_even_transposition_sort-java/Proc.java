public class Proc implements Runnable {

	public Proc(int[] array, int index, MsgBuf[] mq) {
		this.rank = index;
		this.val = array[index];
		this.procs = mq;
		this.result = array;
	}

	public void run() {
		int n = result.length;

		for (int i = 0; i < n; i++) {
			if ((i & 1) == (rank & 1)) { /* processor has active role */
				if (rank < (n-1)) { /* we are not on the outermost right */
					int r = procs[rank].recv();
					if (r < val) {
						procs[rank+1].send(val);
						val = r;
					} else {
						procs[rank+1].send(r);
					}
				}
			} else { /* passive role */
				if (rank > 0) { /* not the left edge */
					procs[rank-1].send(val);
					val = procs[rank].recv();
				}
			}

		}
		result[rank] = val;
	}

	private int rank;
	private int val; // interim value
	private MsgBuf[] procs;
	private int[] result;

}
