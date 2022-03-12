import router from '@system.router';
import pageName from '../stringConst';

@Entry
@Component
struct Second {
  private content: string = pageName.page1;
  mIndex: string
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`${this.content}`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Column() {
        TextInput({placeholder:'please input', text:'wdd'})
          .type(InputType.Normal)
          .enterKeyType(EnterKeyType.Send)
          .onChange((value) => {
            console.log('[dmsDemo] TextInput onChange:' + value);
            this.mIndex = value;
          }).onSubmit((enterKey) => {
          console.log('[dmsDemo] TextInput onSubmit:' + enterKey);
        }).onEditChanged((isEditing) => {
          console.log('[dmsDemo] TextInput onEditChanged:' + isEditing);
        }).width(250).height(50)
        Flex({ justifyContent: FlexAlign.SpaceAround }) {
          Button('cancel')
            .onClick(() => {
              //this.controller.close()
              this.cancel()
            }).backgroundColor(0xffffff).fontColor(Color.Black)
          Button('submit')
            .onClick(() => {
              //this.controller.close()
              this.confirm()
            }).backgroundColor(0xffffff).fontColor(Color.Red)
        }.margin({ bottom: 20 })
      }
      Button() {
        Text(pageName.backKey)
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
      }.type(ButtonType.Capsule)
      .margin({
        top: 60
      })
      .backgroundColor('#0D9FFB')
      .onClick(() => {
        console.log('[dmsDemo] TextInput back:' + this.mIndex);
        router.back()
      })
    }
    .width('100%')
    .height('100%')
  }
}