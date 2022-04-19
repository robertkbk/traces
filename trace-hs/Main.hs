{-# LANGUAGE DeriveGeneric #-}
{-# LANGUAGE OverloadedStrings #-}

import Data.Aeson (genericToEncoding)
import qualified Data.Aeson as Aeson
import qualified Data.ByteString.Lazy.Char8 as ByteString
import Data.Char (isAlpha)
import Data.List (intercalate, intersperse, nub)
import Data.Map (Map, elems, member, notMember, insert, empty, fromList, keys, toList)
import qualified Data.Map (map)
import GHC.Generics (Generic)
import GHC.IO.Handle.FD (stderr)
import System.Environment (getArgs, getProgName)
import System.Exit (exitFailure, exitSuccess)
import System.IO (hPrint)
import Text.Regex (matchRegexAll, mkRegex, Regex, matchRegex)
import Prelude hiding (product)
import Data.Maybe
import Control.Monad (unless)
import Production
import Graph

type BString = ByteString.ByteString

{- 
  Struktura programu:
    - dane wejściowe są deserializowane z formatu JSON z wykorzystaniem biblioteki Aeson
    - na podstawie danych wejściowych jest tworzony program
-}
data InputData = InputData
  { symbols     :: String,
    alphabet    :: String,
    operations  :: Map Char String,
    word        :: String
  }
  deriving (Generic, Show)

instance Aeson.ToJSON InputData where
  toEncoding = genericToEncoding Aeson.defaultOptions

instance Aeson.FromJSON InputData

data Program = Program
  { inputData             :: InputData,
    productions           :: [Production],
    dependenceRelation    :: [(Char, Char)],
    independenceRelation  :: [(Char, Char)]
  }
  deriving (Generic)

-- wypisywanie programu
instance Show Program where
  show p = 
    "D = {" ++ intercalate ", " (map showPair d) ++ "}\n"
      ++ "I = {" ++ intercalate ", " (map showPair i) ++ "}\n"
      ++ "FNF = " ++ concatMap brackets (process p)

    where 
      d = dependenceRelation p
      i = independenceRelation p

      showPair (a, b) =
        "(" ++ [a] ++"," ++ [b] ++ ")"

      brackets x = '[' : x ++ "]"


{-
  Sprawdzanie poprawności danych wejściowych, kolejno:
    - alfabetu
    - operacji
    - słowa nad alfabetem
-}
checkAlphabet :: InputData -> IO InputData
checkAlphabet inputData =
  if all isAlpha $ alphabet inputData
    then return inputData
    else errorWithoutStackTrace "incorrect alphabet (only alpha characters allowed)"

checkOperations :: InputData -> IO InputData
checkOperations inputData = do
  if null missingOps
    then if null invalidOps
      then return inputData
      else errorWithoutStackTrace $ "invalid operations: " ++ intercalate ", " invalidOps
    else errorWithoutStackTrace $ "missing operation definitions: " ++ intercalate ", " (map show missingOps)
  where
    o = operations inputData
    a = alphabet inputData
    s = symbols inputData
    missingOps = filter (`notMember` o) a
    invalidOps = filter checkOp $ elems o
    checkOp op = isNothing $ parse '.' op

checkWord :: InputData -> IO InputData
checkWord inputData =
  if all (`elem` a) w
    then return inputData
    else errorWithoutStackTrace "input: word must contain only letters of the alphabet"
  where
    a = alphabet inputData
    w = word inputData

checkInputData :: InputData -> IO InputData
checkInputData inputData = do
  checkAlphabet inputData >>= checkOperations >>= checkWord

{-
  Wyznaczanie produkcji i relacji (nie) zależności między poszczególnymi produkcjami
-}
parseProductions :: InputData -> Maybe [Production]
parseProductions inputData = do
  mapM (uncurry parse) (toList $ operations inputData)

dependence :: [Production] -> [(Char, Char)]
dependence prod =
  [(symbol x, symbol y) | x <- prod, y <- prod, dependent x y]

independence :: [Production] -> [(Char, Char)]
independence prod =
  [(symbol x, symbol y) | x <- prod, y <- prod, not $ dependent x y]

createProgram :: InputData -> [Production] -> Program
createProgram inputData productions =
  Program inputData productions
    (dependence productions) (independence productions)

{-
  Wyznaczanie postaci normalnej Foaty dla danych zawartych w programie.
  W implementacji wykorzystano algorytm opisany na stronie 10 dokumentu
  dostarczonego do zadania.
-}
process :: Program -> [[Char]]
process program = do
  filter (not . null) . map (filter (/= '*')) $ run norm
  where
    i = inputData program
    a = alphabet i
    w = word i
    d = dependenceRelation program
    preNorm = fromList $ map keyStack a
    maxLength = maximum $ map length $ elems preNorm
    norm = Data.Map.map (`normalizeLength` maxLength) preNorm

    pop m = (map last $ elems m, Data.Map.map init m)

    run m =
      let (x, mm) = pop m
      in if any null $ elems m
        then []
        else x : run mm

    processLetter a b
      | a == b            = a
      | (a, b) `elem` d   = '*'
      | otherwise         = '.'

    proccessAgainstLetter a =
      filter (/= '.') [processLetter a b | b <- reverse w]

    keyStack l =
      (l, proccessAgainstLetter l)

    normalizeLength arr len =
      arr ++ replicate (len - length arr) '*'

{-
  Wyznaczanie grafu zależności w postaci niezminimalizowanej
  (minimalizacja: transitiveReduction [Graph.hs:34])
-}
dependenceGraph :: Program -> Graph
dependenceGraph prog =
    Graph w e
  where
    i = inputData prog
    a = alphabet i
    w = word i
    d = dependenceRelation prog

    rw (x : xs) = (x, xs) : rw xs
    rw [] = []

    e = [(i, j)
          | (i, a) <- zip [0..] w,
            (j, b) <- zip [(i + 1)..] $ drop (i + 1) w,
            (a, b) `elem` d
        ]

    idf v = length w - length v


{-
  Funkcja main
-}
main :: IO b
main = do
  id <- getArgs
    >>= getFilename
    >>= ByteString.readFile
    >>= decode
    >>= checkInputData

  case createProgram id <$> parseProductions id of
    Nothing -> error "program: cannot parse productions"
    Just pro -> do
      print pro
      writeFile "result.dot" (toDot $ transitiveReduction $ dependenceGraph pro)

  exitSuccess

decode :: BString -> IO InputData
decode jsonData =
  case Aeson.decode jsonData of
    Just a -> return a
    Nothing -> errorWithoutStackTrace "json: cannot parse JSON data"

getFilename :: [String] -> IO String
getFilename [] =
  getProgName
    >>= (\name -> return $ "usage: " ++ name ++ " <jsonFile>")
    >>= errorWithoutStackTrace
getFilename (x : xs) = return x