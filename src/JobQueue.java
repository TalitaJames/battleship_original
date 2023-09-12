import java.util.Arrays;
import java.util.concurrent.locks.ReentrantLock;
// import java.util.concurrent.Semaphore;

public class JobQueue{
    private static volatile Byte[] nextByte;
    private static long progress;

    private static final Object lock = new Object();

    // the overal limit of the byte for all boards of this length
    private static final Byte realMin = 0;
    private static final int uRealMax = ((Board.getLength()-1) * 11 << 1) | 0b00000001;
    private static final Byte realMax = (byte) uRealMax;

    public static void initJobQueue(){
        nextByte = new Byte[Runner.getFleetLength()];
        for (int i=0; i<nextByte.length; i++) nextByte[i]=realMin;
        progress=0;
    }

    public synchronized static Byte[] getByte(){
        Byte[] currentByte;
        synchronized(lock){
            boolean validByte = false; 
            if(nextByte==null || checkEndVal(nextByte)) return null;

            // Byte[] currentByte = nextByte;
            currentByte = Arrays.copyOf(nextByte, nextByte.length);
        
            while(!validByte && nextByte!=null){
                // iterate the value
                for (int i = 0; i < nextByte.length; i++) {
                    if (nextByte[i]==realMax){
                        nextByte[i]=realMin;
                    } else{
                        nextByte[i]++;
                        break;
                    }
                }

                // checks if is a valid byte[] (ie all bytes are unique and none are illegal)
                // Byte[] dataEvens = Arrays.copyOf(nextByte, nextByte.length);
                // for (int i = 0; i < dataEvens.length; i++) {
                    // if(dataEvens[i]%2 == 1) dataEvens[i]--; // turn this into an arithmatic thing?
                // }
                if(!(Arrays.stream(nextByte).distinct().count() < nextByte.length)) validByte = true;

                if(checkEndVal(nextByte)) nextByte=null;
            }
        }

        return currentByte;
        
    }

    private static boolean checkEndVal(Byte[] data){ // checks if reached the end of all bytes to check
        boolean endVal = true;
        int n = 0;
        while(endVal && n<data.length){
            endVal = (data[n]==realMax);
            n++;
        }
        return endVal;
    }

}

