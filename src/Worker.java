import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Set;
import java.util.HashSet;
import java.util.Map;
import java.util.HashMap;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.ObjectOutputStream;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.io.IOException;

public class Worker implements Serializable { //FIXME: rename?

    private final Ship[] fleet; // can maybe be static between workers?

    // configuration unwrapped (Should it be [start, end)? currently [start, end]  ) 
    private Byte[] minByte;
    private Byte[] maxByte;

    // Absolute limit variables for this board length
    private static final byte absMin = 0;
    private static final int uAbsMax = ((Board.getLength()-1) * 11 << 1) | 0b00000001;
    private static final byte absMax = (byte) uAbsMax;

    private List<Byte[]> configByte;
    private List<Boolean> configBool;
    
    public Worker(Byte[][] startStop){
        this(startStop[0], startStop[1], Runner.getFleet());
    }

    public Worker(Byte[] minByte, Byte[] maxByte, Ship[] fleet){
        this.minByte=minByte;
        this.maxByte=maxByte;
        this.fleet=fleet;

        configByte = new ArrayList<>();
        configBool = new ArrayList<>();
    }
   
    // Do: Given a config sequence segment [start byte, end byte) check each byte, and make note of when it changes state (from good to bad)
    public long doThing(){ //FIXME: rename 
        boolean stateIsBad = false; // state of segment being explored
            
        long goodBoards = 0; 
        Byte[] runnerByte = Arrays.copyOf(minByte, minByte.length); // the byte that iterates thru each board
        PrimitiveBoard trialBoard = new PrimitiveBoard(runnerByte, fleet); // the board made

        while(runnerByte!=null){
            trialBoard = new PrimitiveBoard(runnerByte, fleet);
            if(trialBoard != null && !trialBoard.getIsBad()) goodBoards++;

            if(stateIsBad != trialBoard.getIsBad()){
                Byte[] copiedByte = Arrays.copyOf(runnerByte, runnerByte.length);
                configByte.add(copiedByte); // add the change of state to the configuration
                configBool.add(trialBoard.getIsBad());

                stateIsBad=trialBoard.getIsBad();
            }
            runnerByte = nextByte(runnerByte); 
        }
        // in the future, this will be threaded, so check for overlapping config information when that happens 
        // and the configBool needs to alternate TFTFTFTF 
        // now return the config and die(?)

        return (long) goodBoards;
    }

    public void testing(){
        System.out.println(configByte.size());
    }

    // Turns the configBytes from 
    public List<Byte[][]> configToSequence(){
        boolean switchState = false; // the state (false) that the board is looking for
        
        List<Byte[][]> sequence = new ArrayList<>();
    
        for(int i=1; i<configByte.size(); i+=2){
            Byte[][] singleConfig = new Byte[2][configByte.get(i).length];
            singleConfig[0] = configByte.get(i);  // starting segment
            singleConfig[0] = configByte.get(i+1);  // end segment
            sequence.add(singleConfig);
        }
        
        return sequence;
    }


    // Split


    // Combine


    // Count: Given a config sequence, return int (number of boards it contains)
    // If [S,E) then 

    // --- Byte Iteration Methods

    private Byte[] nextByte(Byte[] data){
        boolean validByte = false;
        
        while(!validByte){
            for (int i = 0; i < data.length; i++) {
                if (data[i]==absMax){
                    data[i]=absMin;
                } else{
                    data[i]++;
                    break;
                }
            }

            // checks if is a valid byte[] (ie all bytes are unique and none are illegal)
            Set<Byte> ByteSet = new HashSet<>();
            // for (Byte t : data) ByteSet.add((byte) (t/2));
            // if (ByteSet.size() == data.length) validByte = true;

            validByte = true;
            
            if(checkEndVal(data)) return null;
        }
        return data;
    }

    private boolean checkEndVal(Byte[] data){ // checks if at the end value of the sub list to check
        boolean endVal = true;
        int n = 0;
        while(endVal && n<data.length){
            endVal = (data[n]==maxByte[n]);
            n++;
        }
        return endVal;
    }

    // --- IO
    public void serializeWorker(String filename){
        try {
            FileOutputStream fileOut = new FileOutputStream(filename);
            ObjectOutputStream out = new ObjectOutputStream(fileOut);
            out.writeObject(this);
            out.close();
            fileOut.close();
            System.out.println("Serialized data is saved in " + filename);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static Worker deserializeWorker(String filename){
        Worker tmp = null;
        try {
            FileInputStream fileIn = new FileInputStream(filename);
            ObjectInputStream in = new ObjectInputStream(fileIn);
            tmp = (Worker) in.readObject();
            in.close();
            fileIn.close();
        } catch (IOException i) {
            i.printStackTrace();
        } catch (ClassNotFoundException c) {
            System.out.println("Class not found");
            c.printStackTrace();
        }
        return tmp;
    }

    // Save all the state info about this worker obj to a file, so it can be recreated later
    // Serialised?
}