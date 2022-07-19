import nextcord
from nextcord.ext import commands
import os


class mjBot:
    def __init__(self):
        intents = nextcord.Intents.default()
        intents.message_content = True
        self.client = commands.Bot(command_prefix='!', intents=intents)


    def run(self):
        @self.client.command()
        async def ping(ctx):
            await ctx.send('Pong!')
        
        


        self.client.run(os.environ['OPENMJ_DISCORD_TOKEN'])

if __name__ == '__main__':
    bot = mjBot()
    bot.run()