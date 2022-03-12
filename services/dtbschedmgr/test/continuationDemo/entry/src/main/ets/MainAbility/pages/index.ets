import router from '@system.router';
import Context from '@ohos.application.Context';
import pageName from '../stringConst';

async function routePage1() {
  console.log("111");
  let options = {
    uri: 'pages/second'
  }
  try {
    await router.push(options)
  } catch (err) {
    console.error(`fail callback, code: ${err.code}, msg: ${err.msg}`)
  }
}

async function routePage2() {
  console.log("222");
  let options1 = {
    uri: 'pages/third'
  }
  try {
    await router.push(options1)
  } catch (err) {
    console.error(`fail callback, code: ${err.code}, msg: ${err.msg}`)
  }
}

export default class ObjTest {
  public id : number = 0;
  public name: string = "default"
}

@Entry
@Component
struct Index {
//  @StorageLink('objtest') objtest: ObjTest = {id:123, name:"defaulename"};
//  private storage :ContentStorage = getContentStorage(this);
//  private context :Context = getContext(this)
//  ;

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('备忘录')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Button() {
        Text(pageName.page1)
          .fontSize(35)
          .fontWeight(FontWeight.Bold)
      }.type(ButtonType.Normal)
      .margin({
        top: 30
      })
      .backgroundColor('#2cb3cb')
      .onClick(() => {
        routePage1()
      })
      Button() {
        Text(pageName.page2)
          .fontSize(35)
          .fontWeight(FontWeight.Bold)
      }.type(ButtonType.Normal)
      .margin({
        top: 60
      })
      .backgroundColor('#2cb3cb')
      .onClick(() => {
        routePage2()
      })
    }
    .width('100%')
    .height('100%')
  }
}