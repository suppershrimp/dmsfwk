/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { NextFigureObject, NextFigure } from './NextFigure'
import { GameObject, State } from './TetrisViewModel'
import { GameField, FieldObject } from './GameField'
import { Figure, FigureObject, CollisionType } from './Figure'
import { BlockState } from './BlockState'
import { GameState } from './GameState'

export class ModelObject {
  rows: number
  columns: number
  figureObject: FigureObject = new FigureObject
  nextFigureObject: NextFigureObject = new NextFigureObject
  screen: BlockState[][] = []
  fieldObject: FieldObject = new FieldObject
}

export class GameModel {
  gameObject: GameObject
  figure: Figure
  nextFigure: NextFigure
  field: GameField

  constructor(gameObject: GameObject) {
    this.gameObject = gameObject
    this.gameObject.modelObject.rows = this.gameObject.rows
    this.gameObject.modelObject.columns = this.gameObject.columns

    for (var i = 0; i < this.gameObject.modelObject.rows; i++) {
      var scr: BlockState[] = []
      for (var j = 0; j < this.gameObject.modelObject.columns; j++) {
        scr.push(BlockState.empty)
      }
      this.gameObject.modelObject.screen.push(scr)
    }

    this.field = new GameField(this.gameObject)

    this.figure = this.makeFigure(this.gameObject.modelObject.figureObject.typeNum, this.gameObject.modelObject.figureObject.position)
    this.gameObject.modelObject.figureObject = this.figure.gameObject.modelObject.figureObject
    this.nextFigure = this.makeNextFigure(this.gameObject.modelObject.nextFigureObject.typeNum)
    this.gameObject.modelObject.nextFigureObject = this.nextFigure.gameObject.modelObject.nextFigureObject
  }

  makeFigure(typeNum: number, position: number[]): Figure {
    return new Figure(this.gameObject, typeNum, position, this.field)
  }

  makeNextFigure(typeNum: number): NextFigure {
    return new NextFigure(this.gameObject, typeNum)
  }

  clearMap() {
    for (var i = 0; i < this.gameObject.modelObject.columns; i++) {
      for (var j = 0; j < this.gameObject.modelObject.rows; j++) {
        this.gameObject.modelObject.screen[j][i] = BlockState.empty
      }
    }
  }

  move(): GameState {
    if (!this.figure.moveDown()) {
      this.figure.putField()
      this.gameObject.modelObject.fieldObject = this.gameObject.modelObject.figureObject.fieldObject

      var position: number[] = [this.gameObject.columns / 2, this.gameObject.rows - 3]
      this.figure = this.makeFigure(this.nextFigure.gameObject.modelObject.nextFigureObject.typeNum, position)
      this.gameObject.modelObject.figureObject = this.figure.gameObject.modelObject.figureObject
      this.nextFigure = this.makeNextFigure(Math.floor(Math.random() * 7) + 1)
      this.gameObject.modelObject.nextFigureObject = this.nextFigure.gameObject.modelObject.nextFigureObject

      if (this.figure.checkCollision() == CollisionType.down) {
        return GameState.endGame
      }
    }
    var state: GameState = this.field.burning()
    this.figure.gameField = this.field
    this.figure.gameObject.modelObject.fieldObject = this.field.gameObject.modelObject.fieldObject
    return state
  }

  update() {
    this.clearMap()
    this.field.put(this.gameObject.modelObject.screen)
    this.figure.putScreen(this.gameObject.modelObject.screen)
  }
}