import java.util.Map;
import java.util.HashMap;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

public class Board implements Serializable {
    private final static int SIZE = 5; //FIXME this would be from the .json

    private final Map<String, Cell> board;
    private final Map<Ship, Boolean> shipStatus;

    // Initalises a blank board 
    public Board() {
        this.board = new HashMap<>();
        this.shipStatus = new HashMap<>();

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
        int[] shipLen = {2,3}; //FIXME this would be from the .json
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

        return board; //TODO test
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
        shipStatus.put(ship, false);
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
            this.updateStatus();
            return success;
        }
        else throw new InvalidPositionException();
    }


    private void updateStatus(){ // not sure about keeping this method, seems pointless
        for(Ship s: shipStatus.keySet()){
            if (s.sunk()){
                shipStatus.put(s,true);
            }
        }
    }

    // tells you if the game is over (ie all ships are sunk)
    public boolean gameOver() {
        for(Ship s: shipStatus.keySet()){
            if (!s.sunk()) return false;
        }
        return true;
    }

    // a string version of the tbd list that tells you which ships are sunk (for size guessing ect)
    public String getShipStatusString() {
        this.updateStatus();
        String stat ="";
        for (Ship s : shipStatus.keySet()) {
            stat+= s.getSymbol()+" is sunk: "+s.sunk()+"\n";
        }
        if (shipStatus.size()==0){
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
