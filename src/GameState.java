import java.util.Map;
import java.util.HashMap;

/*
 * A wrapper static class to keep variables 
 * related to whole game operation (bytes, fleet ect)
 * 
*/

public class GameState{
    private static Ship[] fleet;
    private static Map<Integer,Boolean> hitmask;
    private static int threadCount;

    public static void setAll(Ship[] fleet, int threadCount, Map<Integer,Boolean> hitmask){
        GameState.setFleet(fleet);
        GameState.setHitmask(hitmask);
        GameState.setThreadCount(threadCount);
    }

    // makes the fleet final
    public static void setFleet(Ship[] fleet){ 
        if (GameState.fleet == null) {
            GameState.fleet = fleet;
        } else {
            throw new IllegalStateException("The fleet has been set already!");
        }
    }
    public static Ship[] getFleet(){ return fleet; }

    public static void setHitmask(Map<Integer,Boolean> hitmask){ GameState.hitmask = hitmask; }
    public static Map<Integer,Boolean> getHitmask(){ return hitmask; }

    public static void setThreadCount(int threadCount){ GameState.threadCount = threadCount; }
    public static int getThreadCount(){ return threadCount; }

    // --- Byte Data ---
    private static final byte MinByte = 0;
    // private static final int unsignedMaxByte = ((Board.getLength()-1) * 11 << 1) | 0b00000001; //FIXME: was evaluating presuamably before the board length is set?


    // --- Byte Methods ---
    public static byte getMinByte(){ return MinByte; }
    public static Byte[] getMinByteArray(){
        Byte[] start = new Byte[GameState.getFleet().length];
        for(int i=0; i<GameState.getFleet().length; i++){ start[i]=getMinByte(); }
        return start;
    }

    public static int getMaxUnsignedByte(){ return  ((Board.getLength()-1) * 11 << 1) | 0b00000001; } // this is confusing but i can't think of something better
    public static byte getMaxByte(){ return (byte) getMaxUnsignedByte(); }
    public static Byte[] getMaxByteArray(){
        Byte[] stop = new Byte[GameState.getFleet().length];
        for(int i=0; i<GameState.getFleet().length; i++){ stop[i]=getMaxByte(); }
        return stop;
    }
}