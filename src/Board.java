import java.util.Map;
import java.util.HashMap;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

public class Board implements Serializable {
    private final static int SIZE = 5;

    private final Map<String, Cell> board;
    private final Map<Ship, Boolean> shipStatus;


    public Board() {
        this.board = new HashMap<>();
        this.shipStatus = new HashMap<>();

        for (int y = 0; y < Board.SIZE; y++) {
            for (int x = 0; x < Board.SIZE; x++) {
                board.put(Board.coord(x, y), new Cell());
            }
        }
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

    public boolean gameOver() {
        for(Ship s: shipStatus.keySet()){
            if (!s.sunk()) return false;
        }
        return true;
    }

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

    public static String coord(int x, int y){
        return "("+x+","+y+")";
    }

    public static int getSize() {
        return Board.SIZE;
    }

    
}
