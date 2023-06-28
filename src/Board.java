import java.util.Map;
import java.util.HashMap;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

public class Board implements Serializable {
    private final static int SIZE = 5; //.json

    private final Map<String, Cell> board;
    private final Map<Ship, String> shipList;

    // Initalises a blank board 
    public Board() {
        this.board = new HashMap<>();
        this.shipList = new HashMap<>();

        for (int y = 0; y < Board.SIZE; y++) {
            for (int x = 0; x < Board.SIZE; x++) {
                board.put(Board.coord(x, y), new Cell());
            }
        }
    }



    public static Board decodeBoard(byte[] shipCodes) 
                throws InvalidPlacementException, InvalidShipTypeException, InvalidPositionException {
        
        Board board = new Board();
        
        // for 5x5 grid [2,3] or 10x10 grid [2,3,3,4,5]
        int[] shipLen = {2,3}; //.json
        char[] shipChar = {'2','3','A','4','5'};

        for (int i = 0; i < shipCodes.length; i++) {
            // Create a new ship following fixed lengths (from rules & )
            Ship newShip = new Ship(shipLen[i], shipChar[i]);
            
            //decoding the byte section
            byte encodedShip = shipCodes[i];
            boolean dir = (encodedShip % 2 != 0); // if odd, then true true
                
            int uint = encodedShip & 0xff; // unsign it
            uint>>=1; // get rid of directional info

            int x= (int) Math.floor(uint/10); // undoes encoding in the form of x*10+y
            int y= uint % 10;
            
            board.placeShip(newShip, Board.coord(x, y), dir);
        }

        return board; //TODO test this code
    }

    //FIXME implement this
    public byte[] encodeBoard() {
        // 1) work out where the ships are
        // 2) put them in the right order to be encoded per the 
        // from .json

        // 3) encode each ships data
        byte[] encodedShipData = new byte[2]; //.json

        for (int i = 0; i < encodedShipData.length; i++) {
            int x=0;
            int y=0;
            boolean dir=false;

            encodedShipData[i]=encodeShip(x, y, dir); 
        }


        return encodedShipData; 
        
    }

    private byte encodeShip(int x, int y, boolean direction) {
        int val = x*10+y;
        byte encoded = (byte) val;
        encoded <<= 1;

        int dirInt = direction ? 1 : 0;
        byte dirByte = (byte) dirInt;
        encoded |= dirByte;

        return encoded;
    }   

    public Board deepCopy() throws IOException, ClassNotFoundException{
        // I do not know how this works, but i do know it is slow and inefficient

        //Serialization of object
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        ObjectOutputStream out = new ObjectOutputStream(bos);
        out.writeObject(this);

        //De-serialization of object
        ByteArrayInputStream bis = new ByteArrayInputStream(bos.toByteArray());
        ObjectInputStream in = new ObjectInputStream(bis);
        Board copied = (Board) in.readObject();
    
        return copied;    
    
    }

    // adds a ship to a board, errors for intersections and overhangs
    public void placeShip(Ship ship, String coord, boolean direction) 
                throws InvalidPlacementException, InvalidShipTypeException, InvalidPositionException {
        if (ship == null) throw new InvalidShipTypeException("Null ship");
        if (!board.containsKey(coord)) throw new InvalidPositionException("Bad possition");

        for (int offset = 0; offset < ship.getLength(); offset++) { // check the ship isn't out of bounds or intersecting
            String nextPosition = getPosPlus(coord, offset, direction);
            if (!board.containsKey(nextPosition) || board.get(nextPosition).isOccupied()){
                throw new InvalidPlacementException("Out of bounds or intersecting ship");
            }
        }

        // place the ship
        shipList.put(ship, coord); //TODO this could have the ship & its byte together then the encode board groups in an array
        for (int offset = 0; offset < ship.getLength(); offset++) {
            board.get(getPosPlus(coord, offset, direction)).placeSegment(ship.getSegment(offset));
        }
    }

    // Gets the next position along from the ships direction
    private String getPosPlus(String coord, int offset, boolean direction) {
        String[] result = coord.replace('(',' ').replace(')',' ').split(",");
        int x = Integer.parseInt(result[0].trim());
        int y = Integer.parseInt(result[1].trim());
        
        if (direction) {
            return Board.coord(x+offset,y);
        }
        return Board.coord(x, y+offset);
    }


    public boolean attack(String coord) throws InvalidPositionException {
        if (board.containsKey(coord)){
            boolean success = board.get(coord).attack();
            return success;
        }
        else throw new InvalidPositionException();
    }

    // tells you if the game is over (ie all ships are sunk)
    public boolean gameOver() {
        for(Ship s: shipList.keySet()){
            if (!s.sunk()) return false;
        }
        return true;
    }

    // a string version of the tbd list that tells you which ships are sunk (for size guessing ect)
    public String getShipStatusString() {
        String stat ="";
        for (Ship s : shipList.keySet()) {
            stat+= s.getSymbol()+" is sunk: "+s.sunk()+"\n";
        }
        if (shipList.size()==0){
            stat="This board doesn't have ships";
        }
        return stat;
        
    }

    public boolean hasBeenHit(String coord) throws InvalidPositionException {
        if (board.containsKey(coord))
            return board.get(coord).hasBeenHit();
        else throw new InvalidPositionException();
    }

    @Override // displays the users progression thru game
    public String toString() {
        
        String grid = "";
        for (int y = 0; y < Board.SIZE; y++) {
            grid += "[";
            for (int x = 0; x < Board.SIZE; x++) {
                grid += board.get(coord(x, y)) + " ";
            }
            grid +="]\n";
        }
        return grid;
    }

    // displays whole grid (not hit/miss data)
    public String displaySetup() {
        String grid = "";
        for (int y = 0; y < Board.SIZE; y++) {
            grid +="[";
            for (int x = 0; x < Board.SIZE; x++) {
                grid += board.get(Board.coord(x, y)).displaySetup() + " ";
            }
            grid +="]\n";
        }
        return grid;
    }

    // returns true if ship there, false if not
    public boolean isOccupied(String coord) {       
        return board.get(coord).isOccupied();
    }

    // Standardized referal of ship corrdinates
    public static String coord(int x, int y){
        return "("+x+","+y+")";
    }

    public static int getSize() {
        return Board.SIZE;
    }


}
