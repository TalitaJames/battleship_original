import java.util.Map;
import java.util.HashMap;

public class Board {
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

    private Board(Map<String, Cell> copyBoard, Map<Ship, Boolean> copyShipStatus) { // for the copy method
        this.board = new HashMap<>(copyBoard);
        this.shipStatus = new HashMap<>(copyShipStatus);
    }

    public static Board deepCopy(Board old) { //FIXME: its currently shallow
        return new Board(old.getBoard(), old.getShipStatus());
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

    public void attack(String coord) throws InvalidPositionException {
        if (board.containsKey(coord)){
            board.get(coord).attack();
            this.updateStatus();
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

    public Map<String, Cell> getBoard() {
        return board;
    }

    public Map<Ship, Boolean> getShipStatus() {
        this.updateStatus();
        return shipStatus;
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
                grid += board.get(coord(x, y)).displaySetup() + " ";
            }
            grid +="]\n";
        }
        return grid;
    }

    public static String coord(int x, int y){
        return "("+x+","+y+")";
    }

    public static int getSize() {
        return SIZE;
    }

  
}
