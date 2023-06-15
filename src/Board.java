import java.util.Map;
import java.util.HashMap;

public class Board {

    private final static int SIZE = 10;
    private final Map<String, Cell> board;

    public Board() {
        board = new HashMap<String, Cell>();
        for (char i = 'A'; i <= 'J'; i++) {
            for (int j = 1; j <= Board.SIZE; j++) {
                board.put(i + "" + j, new Cell());
            }
        }
    }

    public void placeShip(Ship ship, String position, String direction) throws InvalidPlacementException, InvalidShipTypeException, InvalidPositionException {
        if (ship == null) throw new InvalidShipTypeException();
        if (!direction.equalsIgnoreCase("across") && !direction.equalsIgnoreCase("down")) throw new InvalidPlacementException();
        if (!board.containsKey(position.toUpperCase())) throw new InvalidPositionException();
        for (int offset = 0; offset < ship.length(); offset++) {
            String nextPosition = getPosPlus(position, offset, direction);
            if (!board.containsKey(nextPosition) || board.get(nextPosition).isOccupied()) throw new InvalidPlacementException();
        }

        for (int offset = 0; offset < ship.length(); offset++) {
            board.get(getPosPlus(position, offset, direction)).placeSegment(ship.getSegment(offset + 1));
        }
    }

    private String getPosPlus(String position, int offset, String direction) {
        if (direction.equalsIgnoreCase("across")) {
            return (position.charAt(0) + "" + (Integer.parseInt(position.substring(1)) + offset)).toUpperCase();
        }
        
        return (((char)(position.charAt(0) + offset)) + position.substring(1)).toUpperCase();
    }

    public void attack(String position) throws InvalidPositionException {
        if (board.containsKey(position.toUpperCase())) board.get(position.toUpperCase()).attack();
        else throw new InvalidPositionException();
    }

    public boolean hasBeenHit(String position) throws InvalidPositionException {
        if (board.containsKey(position.toUpperCase())) return board.get(position.toUpperCase()).hasBeenHit();
        throw new InvalidPositionException();
    }

    @Override
    public String toString() {
        String grid = "  1 2 3 4 5 6 7 8 9 10\n";
        for (char i = 'A'; i <= 'J'; i++) {
            grid += i + " ";
            for (int j = 1; j < Board.SIZE; j++) {
                grid += board.get(i + "" + j) + " ";
            }
            grid += board.get(i + "" + 10) + "\n";
        }
        return grid;
    }

    public String displaySetup() {
        String grid = "  1 2 3 4 5 6 7 8 9 10\n";
        for (char i = 'A'; i <= 'J'; i++) {
            grid += i + " ";
            for (int j = 1; j < Board.SIZE; j++) {
                grid += board.get(i + "" + j).displaySetup() + " ";
            }
            grid += board.get(i + "" + 10).displaySetup() + "\n";
        }
        return grid;
    }
}
