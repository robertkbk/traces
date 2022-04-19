module Graph
  ( Graph (Graph, verticies, edges),
    toDot,
    transitiveReduction,
  )
where

data Graph = Graph
  { verticies :: [Char],
    edges :: [(Int, Int)]
  }
  deriving (Show)

{-
    Konwersja grafu do formatu DOT
-}
toDot :: Graph -> String
toDot g =
  "digraph FoataNormalForm { \n"
    ++ concat (labelVerticies v)
    ++ concatMap showEdge (edges g)
    ++ "}\n"
  where
    v = verticies g

    labelVertex i a =
      "\tv" ++ show i ++ " [label=" ++ a : "];\n"

    labelVerticies v = zipWith labelVertex [0 ..] v

    showEdge (a, b) =
      "\tv" ++ show a ++ " -> v" ++ show b ++ ";\n"

{-
    Minimalizacja grafu z wykorzystaniem algorytmu transitiveReduction
-}
transitiveReduction :: Graph -> Graph
transitiveReduction g =
  Graph v fe
  where
    v = verticies g
    e = edges g
    l = length v - 1

    transitive (x, y) =
      let connected z = elem (x, z) e && elem (z, y) e
       in not $ any connected [0 .. l]

    fe = filter transitive e
