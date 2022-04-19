module Production (
    Production (symbol, product, operands),
    parse,
    dependent
) where

import Data.List (intercalate)
import Data.Maybe
import Text.Regex
import Prelude hiding (product)

productionRegex :: Regex
productionRegex = mkRegex "([A-z])\\s*=\\s*"

operandRegex :: Regex
operandRegex = mkRegex "^[0-9]*([A-z]+)\\s*[-*+]?\\s*"

data Production = Production
  { symbol    :: Char,
    product   :: Char,
    operands  :: [Char]
  } deriving Show

getOperands :: String -> Maybe [Char]
getOperands right =
  if null right 
    then Just []
    else case matchRegexAll operandRegex right of
      Nothing -> Nothing 
      Just (_, _, after, captures) ->
        if null captures
          then Nothing
          else (head captures ++) <$> getOperands after

parse :: Char -> String -> Maybe Production
parse s str =
  case matchRegexAll productionRegex str of
    Just (_, _, after, captures) ->
      if null captures
        then Nothing
        else  Production s ((head . head) captures) <$> getOperands after
    Nothing -> Nothing

dependent :: Production -> Production -> Bool
dependent x y =
  (product x == product y) 
    || elem (product x) (operands y) 
    || elem (product y) (operands x)