import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
import java.util.Random;
import java.util.Collections;
import java.util.Set;
import java.util.HashSet;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.FileNotFoundException;
import java.io.IOException;


public class ByteIterator implements Runnable {
    // TODO: should these still be Byte not byte?
    // the subsection limits for this object
    private final Byte byteMin;
    private final Byte byteMax;
    
    // the overal limit of the byte for all boards of this length
    private static final Byte realMin=0;
    private static final Byte realMax=(byte) ((Board.getLength()-1) * 11 << 1);
    // FIXME: how does java evaluate finals with (hypotheticaly) non static equations in them?

    public ByteIterator(Byte byteMin, Byte byteMax){
        this.byteMin = byteMin;
        this.byteMax = byteMax;
    }  

    @Override
    public void run(){
        Ship[] fleet = new Ship[2];
        fleet[0] = new Ship(2,'2');
        fleet[1] = new Ship(3,'3');
        iterateBytes(fleet);
        
        ByteIterator.threadMessage("I'm Done!");
    }


    // ---- Generating Board Byte Methods
    public long iterateBytes(Ship[] fleet){
        
        Byte[] encodedBoard = new Byte[fleet.length];
        for (int i = 0; i < encodedBoard.length; i++) encodedBoard[i]=byteMin;

        int allBoards=0;
        int goodBoards=0;

        long startTime = System.currentTimeMillis();
        while(encodedBoard!=null){
            // if(allBoards%1e7==0) ByteIterator.threadMessage("40 more "+Arrays.toString(encodedBoard));
            try {
                Board test = Board.decodeBoard(encodedBoard, fleet);
                goodBoards++;
            } catch (InvalidIntersectionException e){
                // System.err.print(" Intersection");
            } catch (InvalidPlacementException e) {
                // System.err.print(" Placement");
            } catch (InvalidShipTypeException e) {
                System.err.println("Uhoh! Ship Type is wrong");
                System.err.println(e.getStackTrace());
            } catch (InvalidPositionException e){
                // System.err.println("Uhoh: Bad coordinate");
            }

            allBoards++;
            encodedBoard = nextByte(encodedBoard);
        }
        long endTime = System.currentTimeMillis();
         
        // return (endTime - startTime);
        return (long) goodBoards;
    }


    private Byte[] nextByte(Byte[] data){
        boolean validByte = false;
        
        while(!validByte){
            // Check if at the end of the values
            boolean endVal = true;
            int n = 0;
            while(endVal && n<data.length){
                endVal = (data[n]==byteMax);
                n++;
            }
            if(endVal) return null;

       
            // iterate the value
            for (int i = 0; i < data.length; i++) {
                if (data[i]==realMax){
                    data[i]=realMin;
                } else{
                    data[i]++;
                    break;
                }
            }

            // checks if is a valid byte[] (ie all bytes are unique and none are illegal)
            if(!(Arrays.stream(data).distinct().count() < data.length)) validByte = true;
        }
    
        return data;
    }

    // ----  Helper misc
    public static int byteToUint(byte data){
        return (int) data & 0b11111111;
    }

    // Display a message, preceded by the name of the current thread
    public static void threadMessage(String message) {
        String threadName = Thread.currentThread().getName();
        System.out.format("%s: %s%n",threadName,message);
    }
}
